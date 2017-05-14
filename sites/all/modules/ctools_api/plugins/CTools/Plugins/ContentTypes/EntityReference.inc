<?php

namespace CTools\Plugins\ContentTypes;

use CTools\PluginContexts;
use CTools\Form\Elements\Collection;

/**
 * Class EntityReference.
 *
 * @package CTools\Plugins\ContentTypes
 */
class EntityReference extends ContentTypesPlugin {

  /**
   * {@inheritdoc}
   */
  public static function name() {
    return t('Entity Reference');
  }

  /**
   * {@inheritdoc}
   */
  public static function contexts() {
    return [
      new \ctools_context_optional(t('Context'), 'any'),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function dependencies() {
    return ['entityreference_autocomplete'];
  }

  /**
   * {@inheritdoc}
   */
  public static function preprocess(array &$variables, array &$arguments, PluginContexts $contexts) {

  }

  /**
   * {@inheritdoc}
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    // "0" is any selected context.
    if (!empty($conf['context'][0])) {
      if ($contexts->has($conf['context'][0])) {
        $context_info = $contexts->get($conf['context'][0])->info();

        foreach ($conf['references'] as &$item) {
          $item['entity_type'] = $context_info->keyword;
        }
      }
    }

    static::entityReferenceCollection(['references'], $form, $form_state, $conf);
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormElementCallback(array &$form, array &$form_state, array &$conf, PluginContexts $contexts, array &$commands, array $element) {
    switch (reset($element['#parents'])) {
      // Clean reference value when context changed.
      case 'context':
        foreach (element_children($form['references']) as $child) {
          if ('_last' !== $child) {
            // Hide field and remove its value.
            static::elementVisibility($form['references'][$child]['reference'], TRUE);
          }
        }
        break;

      // Process only when AJAX occurred on element from "references".
      case 'references':
        // Remove field name from parents to be able obtain full row.
        $item =& drupal_array_get_nested_value($form, array_slice($element['#parents'], 0, -1));
        $field = end($element['#parents']);

        if ('reference' !== $field) {
          switch ($field) {
            // Type was selected - hide the field for selecting item.
            case 'entity_type':
              static::elementVisibility($item['reference'], TRUE);
              break;

            // Bundle was selected - show the field for selecting item.
            case 'bundles':
              static::elementVisibility($item['reference'], FALSE);
              break;
          }
        }
        break;
    }
  }

  /**
   * Create a collection of entity references.
   *
   * {@inheritdoc}
   *
   * @see Collection
   */
  protected static function entityReferenceCollection(array $parents, array &$form, array &$form_state, array $conf) {
    self::initFormState($form_state);

    new Collection($parents, $form, $form_state, $conf, function ($i, $data) use ($form_state) {
      return static::referenceItem($form_state, $data);
    });
  }

  /**
   * Create single entity reference element.
   *
   * {@inheritdoc}
   *
   * @see Collection
   */
  protected static function entityReferenceElement(array $parents, array &$form, array &$form_state, array $conf) {
    self::initFormState($form_state);

    $item = ['#tree' => TRUE];
    $item += static::referenceItem($form_state, drupal_array_get_nested_value($conf, $parents) ?: $conf);

    drupal_array_set_nested_value($form, $parents, $item);
  }

  /**
   * {@inheritdoc}
   */
  protected static function referenceItem(array $form_state, array $data) {
    $item = [];

    $item['entity_type'] = [
      '#ajax' => TRUE,
      '#type' => 'select',
      '#title' => t('Entity'),
      '#options' => $form_state['entity_types'],
      '#empty_option' => t('- None -'),
    ];

    $item['bundles'] = [
      '#ajax' => TRUE,
      '#type' => 'select',
      '#title' => t('Bundles'),
      '#options' => [],
      '#multiple' => TRUE,
    ];

    $item['reference'] = [
      '#type' => 'entityreference',
      '#title' => t('Item'),
      '#multiple' => TRUE,
    ];

    if (!static::elementVisibility($item['bundles'], empty($data['entity_type']))) {
      foreach ($form_state['entity_info'][$data['entity_type']]['bundles'] as $bundle => $info) {
        $item['bundles']['#options'][$bundle] = $info['label'];
      }
    }

    if (!static::elementVisibility($item['reference'], empty($data['bundles']))) {
      $item['reference'] = array_merge($item['reference'], [
        '#era_bundles' => $data['bundles'],
        '#era_entity_type' => $data['entity_type'],
      ]);
    }

    return $item;
  }

  /**
   * Visually show/hide an element.
   *
   * @param array $element
   *   Element to process.
   * @param bool $invisible
   *   Whether element should be visible or not.
   * @param string $class
   *   CSS class for hiding the element.
   *
   * @return bool
   *   Whether element should be visible or not.
   */
  protected static function elementVisibility(array &$element, $invisible, $class = 'element-invisible') {
    // Ensure that element contains attributes.
    $element += ['#attributes' => []];
    // Ensure that attributes contains CSS classes.
    $element['#attributes'] += ['class' => []];

    // Ensure that CSS classes - is an array.
    if (!is_array($element['#attributes']['class'])) {
      $element['#attributes']['class'] = explode(' ', $element['#attributes']['class']);
    }

    $class_position = array_search($class, $element['#attributes']['class']);

    if ($invisible) {
      // Add class if it's not already in list.
      if (FALSE === $class_position) {
        $element['#attributes']['class'][] = $class;
      }

      // Remove value alongside with hiding the element.
      unset($element['#value']);
    }
    else {
      // Remove class if it's in list.
      if (FALSE !== $class_position) {
        unset($element['#attributes']['class'][$class_position]);
      }
    }

    return $invisible;
  }

  /**
   * Store entity types and information in form state.
   *
   * @param array $form_state
   *   Drupal form state.
   */
  private static function initFormState(array &$form_state) {
    if (empty($form_state['entity_info'])) {
      $form_state['entity_info'] = entity_get_info();
    }

    if (empty($form_state['entity_types'])) {
      $form_state['entity_types'] = [];

      foreach ($form_state['entity_info'] as $type => $info) {
        $form_state['entity_types'][$type] = $info['label'];
      }
    }
  }

}
