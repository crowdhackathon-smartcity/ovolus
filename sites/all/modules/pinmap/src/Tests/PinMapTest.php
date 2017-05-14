<?php

namespace Drupal\pinmap\Tests;

if (!module_exists('simpletest')) {
  return;
}

/**
 * Class PinMapTest.
 */
class PinMapTest extends \DrupalWebTestCase {

  /**
   * {@inheritdoc}
   */
  public static function getInfo() {
    return [
      'name' => 'Pin Map',
      'group' => 'pinmap',
      'description' => t('Testing import/export and main configuration forms of the Pin Map module.'),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function setUp() {
    parent::setUp('pinmap', 'field_ui');
  }

  /**
   * Test configuration of the default content type, provided by this module.
   */
  public function testDefaultContentTypeConfiguration() {
    // Go to editing field of the content type.
    $this->isDefaultContentTypeAvailable();

    // Make sure that necessary options selected.
    foreach (['address', 'phone', 'organisation'] as $field) {
      $this->assertFieldChecked("edit-instance-widget-settings-format-handlers-$field");
    }
  }

  /**
   * Test main configuration form.
   */
  public function testMainForm() {
    // @see system_settings_form()
    $button = t('Save configuration');

    // Visit configuration page.
    $this->visit(PINMAP_ADMIN_PATH, ['administer pinmap']);

    // @see pinmap_default_content_type_disabled()
    $field_name = 'pinmap_default_content_type_disabled';
    $field_id = drupal_clean_css_identifier("edit-$field_name");

    // Check default state of disabling default content type - must be enabled.
    $this->assertNoFieldChecked($field_id);
    // Click on the checkbox and submit form.
    $this->drupalPost(PINMAP_ADMIN_PATH, [$field_name => TRUE], $button);
    // Ensure that option saved properly.
    $this->assertFieldChecked($field_id);

    // Check validation of third-party services API keys. Note that
    // error messages should not be wrapped in "t()" function.
    foreach ([
      'Google' => [
        'Invalid API key' => 'The provided API key is invalid.',
        'AIzaSyCb2MUzeAL1fejcUvmtpg3wiF3ar-wQwSI' => 'This API project is not authorized to use this API.',
      ],
      'Yandex' => [
        'Invalid API key' => 'invalid key',
      ],
      'MapQuest' => [
        'Invalid API key' => 'The AppKey submitted with this request is invalid.',
      ],
    ] as $provider => $values) {
      $class = pinmap_provider('Ordzhonikidze', $provider);
      $api_key_field_name = $class::variable('api_key');

      foreach ($values as $input_value => $expected_message) {
        $this->drupalPost(PINMAP_ADMIN_PATH, [
          'pinmap_provider' => $provider,
          $api_key_field_name => $input_value,
        ], $button);

        $this->assertRaw($expected_message);
      }
    }

    // Clean caches to be sure that we had an affect on "hook_node_info()".
    $this->resetAll();
    // Default content type must not be available (404 HTTP status code).
    $this->isDefaultContentTypeAvailable(404);

    // Operations are not available when correctly
    // configured content types does not exists.
    foreach (pinmap_operation_titles() as $operation => $title) {
      $this->isOperationAvailable($operation, 403);
    }
  }

  /**
   * Test importing addresses.
   */
  public function testImport() {
    $data = self::initOperation();
    $iteration = 0;
    $conformity = [];

    // Check that form for importing is ready and available.
    $this->isOperationAvailable('import');
    // Create CSV file for testing.
    $this->createCsv($data->files['import'], $data->columns, $data->address);
    // Set destination field and go to the next step.
    $this->choseOperationField();

    // Build HTML names of the fields.
    foreach ($data->columns as $column => $label) {
      $conformity["conformity[$iteration][source]"] = $label;
      $conformity["conformity[$iteration][destination]"] = $column;
      $iteration++;
    }

    foreach ([
      // Use prepared CSV.
      t('Upload') => ['files[file]' => drupal_realpath($data->files['import'])],
      t('Import') => $conformity,
    ] as $submit => $values) {
      $this->drupalPost(NULL, $values, $submit);
    }

    $this->nodeExist($data->address);
  }

  /**
   * Test exporting addresses.
   */
  public function testExport() {
    $data = self::initOperation();
    $fields = [];

    // Check that we can add new addresses.
    // "a_b" is not equal to "a-b" for "node/add", but works content types:
    // "admin/structure/types/manage/a_b" equals to "admin/structure/types/
    // manage/a-b".
    $this->visit('node/add/' . str_replace('_', '-', PINMAP_DEFAULT_CONTENT_TYPE));

    // Address form depends on country. When we chose one from
    // a list then AJAX request responds the rest of form.
    $trigger = self::fieldName('country');
    $this->drupalPostAJAX(NULL, [$trigger => $data->address['country']], $trigger);

    // Build HTML names of the fields.
    foreach ($data->address as $column => $value) {
      $fields[self::fieldName($column)] = $value;
    }

    // Save a node.
    $this->drupalPost(NULL, $fields, t('Save'));
    // Ensure that node with an address was created properly.
    $this->nodeExist($data->address);

    // Check that form for exporting is ready and available.
    $this->isOperationAvailable('export');
    // Set source field and go to the next step.
    $this->choseOperationField();
    // Make sure that confirmation message is displayed.
    $this->assertRaw(t('Are you sure you want to export 1 nodes?'));
    // Run export of previously created node.
    $this->drupalPost(NULL, [], t('Export'));
    // Create CSV with the same data what we used earlier for node creation.
    $this->createCsv($data->files['import'], $data->columns, $data->address);
    // Make sure that newly created CSV with data for import is
    // fully equal to the file that was created after export.
    $this->assertEqual(file_get_contents($data->files['export']), file_get_contents($data->files['import']));
    // Select source to export from to see if existing file available.
    $this->drupalPost(NULL, [], t('Continue'));
    // Make sure that message with a link on a latest file is displayed.
    $this->assertRaw(t('Latest exported file available here: <a href="@url">@url</a>', [
      '@url' => file_create_url($data->files['export']),
    ]));
  }

  /**
   * Make sure that we have(not) an access to operations configuration form.
   *
   * @param string $operation
   *   Operation name ("import" or "export").
   * @param int $code
   *   Expected HTTP status code.
   */
  private function isOperationAvailable($operation, $code = 200) {
    $this->visit(PINMAP_ADMIN_PATH . '/' . $operation, ["access pinmap $operation"], $code);
  }

  /**
   * Make sure that content type, provided by this module, exists.
   *
   * @param int $code
   *   Expected HTTP status code.
   */
  private function isDefaultContentTypeAvailable($code = 200) {
    // If this page is available, then content type and field exists
    // and all that we need to do - check significant settings.
    $this->visit(sprintf(
      'admin/structure/types/manage/%s/fields/%s',
      PINMAP_DEFAULT_CONTENT_TYPE,
      PINMAP_DEFAULT_FIELD_NAME
    ), [], $code);
  }

  /**
   * Visit URL as authorized user with granted permissions and check HTTP code.
   *
   * @param string $url
   *   Desired URL.
   * @param string[] $permissions
   *   Additional permissions.
   * @param int $code
   *   Expected HTTP status code.
   */
  private function visit($url, array $permissions = [], $code = 200) {
    $this->drupalLogin($this->drupalCreateUser(array_merge($permissions, [
      'administer nodes',
      'administer fields',
      'bypass node access',
      'access content overview',
      'administer content types',
    ])));

    $this->drupalGet($url);
    $this->assertResponse($code);
  }

  /**
   * Make sure that imported/created node exists with correct values.
   *
   * @param array $address
   *   Dummy values returned by pinmap_address_columns(TRUE).
   *
   * @see pinmap_address_columns()
   */
  private function nodeExist(array $address) {
    // SimpleTest works with a clean database, so node ID is always be the "1".
    $this->visit('node/1/edit');

    foreach ($address as $column => $value) {
      $field = self::fieldName($column);

      $this->assertFieldByXPath($this->constructFieldXpath('name', $field), $value, "$field contains '$value' value.");
    }
  }

  /**
   * Create dummy CSV.
   *
   * @param string $filename
   *   Value of the pinmap_get_file_name().
   * @param string[] $columns
   *   CSV header.
   * @param string[] $address
   *   CSV line.
   *
   * @see pinmap_get_file_name()
   */
  private function createCsv($filename, array $columns, array $address) {
    $this->assertTrue(file_put_contents($filename, implode("\n", [
      implode(',', $columns),
      implode(',', $address),
    ])));
  }

  /**
   * First step for the import/export operation - choosing source/destination.
   */
  private function choseOperationField() {
    // Use default values for destination.
    // NOTE: this is the first step of the multi-step form.
    $this->drupalPost(NULL, ['field' => PINMAP_DEFAULT_CONTENT_TYPE . ':' . PINMAP_DEFAULT_FIELD_NAME], t('Continue'));
  }

  /**
   * Collect initial values which needed for import/export operation.
   *
   * @throws \Exception
   *   When dummy address is incorrect.
   *
   * @return \stdClass
   *   ->columns - conformity of field names and labels.
   *   ->address - conformity of field names and dummy values.
   *   ->files
   *     [import] - the name of file for storing info for import.
   *     [export] - the name of file for storing info for export.
   */
  private static function initOperation() {
    $data = [];

    // @code
    // ['postal_code' => 'Post code']
    // @endcode
    $data['columns'] = pinmap_address_columns();
    // @code
    // ['postal_code' => '13349']
    // @endcode
    $data['address'] = pinmap_address_columns(TRUE);
    $data['files'] = [];

    foreach (pinmap_operation_titles() as $operation => $title) {
      $data['files'][$operation] = pinmap_get_file_name(PINMAP_DEFAULT_CONTENT_TYPE, PINMAP_DEFAULT_FIELD_NAME, $operation);
    }

    return (object) $data;
  }

  /**
   * Generate "name" attribute for the field of "addressfield" widget.
   *
   * @param string $column
   *   One of keys from pinmap_address_columns().
   * @param int $i
   *   Field cardinality.
   *
   * @return string
   *   HTML field name.
   *
   * @see pinmap_address_columns()
   */
  private static function fieldName($column, $i = 0) {
    return sprintf("%s[%s][$i][$column]", PINMAP_DEFAULT_FIELD_NAME, LANGUAGE_NONE);
  }

}
