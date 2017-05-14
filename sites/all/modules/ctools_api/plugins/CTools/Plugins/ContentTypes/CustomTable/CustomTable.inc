<?php

namespace CTools\Plugins\ContentTypes\CustomTable;

use CTools\PluginContexts;
use CTools\Form\Elements\Collection;
use CTools\Plugins\ContentTypes\ContentTypesPlugin;

/**
 * Class CustomTable.
 *
 * @package CTools\Plugins\ContentTypes\CustomTable
 */
class CustomTable extends ContentTypesPlugin {

  /**
   * {@inheritdoc}
   */
  public static function name() {
    return t('Custom table');
  }

  /**
   * {@inheritdoc}
   */
  public static function preprocess(array &$variables, array &$arguments, PluginContexts $contexts) {
    $variables['head'] = $variables['head']['head'];
    $variables['columns'] = $variables['head']['columns'];

    unset($variables['head']['columns']);
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    $columns = drupal_array_get_nested_value($conf, ['head', 'head', 'columns']) ?: 2;

    $form['head'] = [
      '#type' => 'table',
      '#sticky' => FALSE,
      '#attributes' => [
        'class' => ['table-rows'],
      ],
    ];

    for ($column = 0; $column < $columns; $column++) {
      $form['head']['head'][$column]['align'] = [
        '#type' => 'radios',
        '#default_value' => 'left',
        '#options' => [
          'left' => t('Left'),
          'center' => t('Center'),
          'right' => t('Right'),
        ],
      ];

      $form['head']['head'][$column]['head'] = [
        '#type' => 'textfield',
      ];

      $form['head']['#header'][] = t('Column #@number', [
        '@number' => $column + 1,
      ]);
    }

    $form['head']['#header'][] = t('Columns');
    $form['head']['head']['columns'] = [
      '#ajax' => TRUE,
      '#type' => 'select',
      '#options' => drupal_map_assoc([1, 2, 3, 4, 5]),
      '#default_value' => $columns,
    ];

    new Collection(['body'], $form, $form_state, $conf, function () use ($columns) {
      $item = [];

      for ($column = 0; $column < $columns; $column++) {
        $item[] = [
          '#type' => 'textfield',
          '#required' => TRUE,
          '#title' => t('Value #@number', [
            '@number' => $column + 1,
          ]),
        ];
      }

      return $item;
    });
  }

}
