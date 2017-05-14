<?php

namespace CTools\Form\Elements;

/**
 * Class Collection.
 *
 * @package CTools\Form\Elements
 */
class Collection {

  const TYPE = 'collection';

  /**
   * Button names. Texts can be changed before creation.
   *
   * @var string[]
   */
  public static $buttonTexts = [
    'remove' => 'Remove',
    'add' => 'Add new item',
  ];

  /**
   * Helper for creating a fields collection.
   *
   * @param string[] $parents
   *   Nesting names.
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Values from configuration form.
   * @param callable $process
   *   Callback for processing each item in a collection.
   */
  public function __construct(array $parents, array &$form, array &$form_state, array &$conf, callable $process) {
    // Pass button texts to translate interface.
    static::$buttonTexts = array_map('t', static::$buttonTexts);

    $name = ctools_api_html_name_from_array($parents);
    // Check that we already have saved values for our collection.
    $values = drupal_array_get_nested_value($conf, $parents);
    $tabledrag = 'panel-weight';
    $element = [
      '#tree' => TRUE,
      '#type' => 'table',
      '#sticky' => FALSE,
      '#header' => [],
      '#subtype' => self::TYPE,
      '#attributes' => [
        'class' => ['form-item', self::TYPE],
      ],
    ];

    // If it's a new collection, then init it with a single, empty, value.
    if (NULL === $values) {
      $values = [[]];
    }

    // Walk through the collection items.
    foreach ($values as $i => $value) {
      $items = $process($i, $value);
      $children = element_children($items);

      if (empty($children)) {
        throw new \RuntimeException(t('Callback for collection must return an associative array of form items.'));
      }

      // Populate properties.
      foreach (element_properties($items) as $prop) {
        if (isset($element[$prop]) && is_array($element[$prop])) {
          $element[$prop] = array_merge($element[$prop], $items[$prop]);
        }
        else {
          $element[$prop] = $items[$prop];
        }
      }

      if ('table' === $element['#type']) {
        // Remove titles from elements and set them to table cells.
        foreach ($children as $td) {
          // Extend an item with an #access property. If it absent - Drupal
          // thinks that an access granted. So, this done to not check for
          // FALSE and isset().
          $items[$td] += ['#access' => TRUE];

          // Do not add header if an access was not granted to an element.
          if ($items[$td]['#access']) {
            $element['#header'][$td] = isset($items[$td]['#title']) ? $items[$td]['#title'] : '';
          }

          unset($items[$td]['#title']);
        }

        // Add the "Remove" button to each row.
        $element['#header']['_operations'] = t('Operations');
        $element['#header']['_weight'] = t('Weight');
      }

      // Could be "fieldset" or "table".
      $element['#attributes']['class'][] = $element['#type'] . '-' . self::TYPE;

      $items['_operations'] = [
        '#type' => 'items',
        '#cell' => [
          'class' => ['operations'],
        ],
      ];

      $items['_operations']['_remove'] = [
        '#ajax' => TRUE,
        // This value MUST BE UNIQUE for every row. Otherwise we always will
        // have the button from last row in $form_state['triggering_element'].
        '#name' => $name . "[$i]",
        '#type' => 'button',
        '#value' => static::$buttonTexts['remove'],
        '#operation' => 'remove',
        '#limit_validation_errors' => [],
      ];

      $items['weight'] = [
        '#type' => 'weight',
        '#default_value' => isset($value['weight']) ? $value['weight'] : 0,
        '#attributes' => [
          'class' => [$tabledrag],
        ],
      ];

      $items['#attributes'] = [
        'class' => ['draggable'],
      ];

      $element[] = $items;
    }

    $element['_last']['_add'] = [
      '#ajax' => TRUE,
      '#name' => $name,
      '#type' => 'button',
      '#value' => static::$buttonTexts['add'],
      '#operation' => 'add',
      '#limit_validation_errors' => [],
      '#cell' => [
        'colspan' => count($element['#header']),
      ],
    ];

    // Set an ID here to be sure that it will not be changed.
    $element['#attributes']['id'] = sprintf('table_%s_%s', self::TYPE, implode('_', $parents));

    // Attach a tabledrag library.
    drupal_add_tabledrag($element['#attributes']['id'], 'order', 'sibling', $tabledrag);
    // Attach a collection, that was generated, to our form.
    drupal_array_set_nested_value($form, $parents, $element);
    // Update form values to able to add new elements to collection.
    drupal_array_set_nested_value($conf, $parents, $values);

    // @todo Remove this block of code if #2836827 will be committed to core.
    // @link https://www.drupal.org/node/2836827
    if (variable_get('ctools_api_fix_tabledrag', TRUE)) {
      // Override some methods of "Drupal.tableDrag" to fix its wrong behavior.
      drupal_add_js(CTOOLS_API_MODULE_PATH . '/js/overrides/tabledrag.js');
    }
  }

}
