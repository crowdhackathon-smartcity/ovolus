<?php

namespace CTools\Plugins\Styles;

use CTools\Form\Elements\Collection;

/**
 * Trait Customizable.
 *
 * @package CTools\Plugins\Styles
 */
trait Customizable {

  /**
   * Process settings.
   *
   * @param string[] $parents
   *   Nesting names.
   * @param array $settings
   *   Values from configuration form.
   */
  protected static function processSettings(array $parents, array &$settings) {
    $values =& drupal_array_get_nested_value($settings, $parents);
    $attributes = [];

    foreach ($values['attributes'] as $i => $item) {
      $attributes[$item['attribute']] = $item['value'];
    }

    $values['attributes'] = array_filter($attributes);
  }

  /**
   * Construct form item for configuring HTML tag and attributes.
   *
   * @param string[] $parents
   *   Nesting names.
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Values from configuration form.
   * @param string[] $tags
   *   HTML tag names.
   */
  protected static function formItem(array $parents, array &$form, array &$form_state, array $conf, array $tags) {
    $tag_parents = array_merge($parents, ['tag']);
    $tag_parents_string = ctools_api_html_name_from_array($tag_parents);

    drupal_array_set_nested_value($form, $tag_parents, [
      '#type' => 'select',
      '#title' => t('HTML tag'),
      '#options' => drupal_map_assoc($tags),
      '#empty_option' => t('- None -'),
    ]);

    new Collection(array_merge($parents, ['attributes']), $form, $form_state, $conf, function () use ($tag_parents_string) {
      $item = [
        '#states' => [
          'invisible' => [
            ":input[name='$tag_parents_string']" => ['value' => ''],
          ],
        ],
      ];

      $item['attribute'] = [
        '#type' => 'textfield',
        '#title' => t('Attribute'),
      ];

      $item['value'] = [
        '#type' => 'textfield',
        '#title' => t('Value'),
      ];

      return $item;
    });
  }

}
