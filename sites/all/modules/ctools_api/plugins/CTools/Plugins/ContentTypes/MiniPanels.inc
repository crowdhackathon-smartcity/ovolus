<?php

namespace CTools\Plugins\ContentTypes;

use CTools\PluginContexts;

/**
 * Class MiniPanels.
 *
 * @package CTools\Plugins\ContentTypes
 */
class MiniPanels extends ContentTypesPlugin {

  const TABLEDRAG_GROUP = 'item-weight';

  /**
   * {@inheritdoc}
   */
  public static function name() {
    return t('Mini panels');
  }

  /**
   * {@inheritdoc}
   */
  public static function dependencies() {
    return ['panels', 'panels_mini'];
  }

  /**
   * {@inheritdoc}
   */
  public static function preprocess(array &$variables, array &$arguments, PluginContexts $contexts) {
    ctools_include('context');

    $raw_contexts = $contexts->getRawArgument();

    foreach ($variables['panels'] as $machine_name => $config) {
      $panel = panels_mini_load($machine_name);
      $panel->display->args = $arguments;

      if (!empty($raw_contexts)) {
        $panel->context = $panel->display->context = ctools_context_load_contexts(
          $panel, FALSE, ctools_context_match_required_contexts($panel->requiredcontexts, $raw_contexts)
        );
      }

      $variables['panels'][$machine_name] = [
        'title' => $panel->admin_title,
        'content' => panels_render_display($panel->display),
      ];
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    $operations = [
      'basic' => t('Settings'),
      'context' => t('Context'),
      'layout' => t('Layout'),
      'content' => t('Content'),
    ];

    $form['panels'] = [
      '#type' => 'table',
      '#sticky' => FALSE,
      '#attributes' => [
        'id' => 'panels',
        'class' => ['form-item'],
      ],
    ];

    foreach (static::getPanels($conf) as $machine_name => $panel) {
      if (empty($panel) || !empty($panel['disabled'])) {
        continue;
      }

      $links = [];
      $row = [];

      foreach ($operations as $tail => $title) {
        $links[] = [
          // @see panels_mini_panels_mini_content_type_admin_info()
          'href' => "admin/structure/mini-panels/list/$machine_name/edit/$tail",
          'title' => $title,
          'attributes' => ['target' => '_blank'],
        ];
      }

      $row['selected'] = [
        '#type' => 'checkbox',
        '#default_value' => !empty($panel['selected']),
      ];

      $row['panel'] = [
        '#markup' => $panel['admin_title'],
      ];

      $row['operations'] = [
        '#input' => FALSE,
        '#links' => $links,
        '#theme' => 'links',
        '#attributes' => [
          'class' => ['inline'],
        ],
      ];

      $row['weight'] = [
        '#type' => 'weight',
        '#default_value' => isset($panel['weight']) ? $panel['weight'] : 0,
        '#attributes' => [
          'class' => [static::TABLEDRAG_GROUP],
        ],
      ];

      $row['#attributes'] = [
        'class' => ['draggable'],
      ];

      $form['panels'][$machine_name] = $row;
    }

    drupal_add_tabledrag($form['panels']['#attributes']['id'], 'order', 'sibling', static::TABLEDRAG_GROUP);
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormSubmit(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    foreach ($conf['panels'] as $machine_name => $panel) {
      // Panel was not selected for displaying.
      if (empty($panel['selected'])) {
        unset($conf['panels'][$machine_name]);
      }
    }

    static::sortPanels($conf);
  }

  /**
   * Returns all panels.
   *
   * @param array $conf
   *   Plugin configuration.
   *
   * @return array[]
   *   List of panels.
   */
  protected static function getPanels(array $conf) {
    $conf += ['panels' => []];

    foreach (panels_mini_load_all() as $machine_name => $panel) {
      $conf['panels'] += [$machine_name => []];
      $conf['panels'][$machine_name] = array_merge((array) $panel, $conf['panels'][$machine_name]);
    }

    static::sortPanels($conf);

    return $conf['panels'];
  }

  /**
   * Sorting panels by weight and usage status.
   *
   * @param array $conf
   *   Plugin configuration.
   */
  protected static function sortPanels(array &$conf) {
    uasort($conf['panels'], function (array $a, array $b) {
      $a_selected = isset($a['selected']) ? $a['selected'] : 0;
      $b_selected = isset($b['selected']) ? $b['selected'] : 0;

      // If they are both selected, or both not-selected, sort by weight.
      return $a_selected == $b_selected ? drupal_sort_weight($a, $b) : ($a_selected > $b_selected ? -1 : 1);
    });
  }

}
