<?php

namespace Drupal\pinmap\CTools\ContentTypes;

use CTools\PluginContexts;
use CTools\Plugins\ContentTypes\ContentTypesPlugin;
use Drupal\pinmap\Geocoder\Providers\Google;

/**
 * Panel with map, markers and search.
 */
class GoogleMap extends ContentTypesPlugin {

  const HOOK_ASSETS = 'pinmap_pane_assets';
  const GOOGLE_MAPS_URL = '//maps.googleapis.com/maps/api/js';
  const STYLES_EDITOR_URL = 'http://www.mapstylr.com/map-style-editor';

  /**
   * Internal count of usage of this content type on the page.
   *
   * @var int
   */
  private static $i = 0;

  /**
   * {@inheritdoc}
   */
  public static function name() {
    return t('Google Map');
  }

  /**
   * {@inheritdoc}
   */
  public static function info() {
    return [
      'category' => t('Pin Map'),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function dependencies() {
    return ['image'];
  }

  /**
   * {@inheritdoc}
   */
  public static function preprocess(array &$variables, array &$arguments, PluginContexts $contexts) {
    $pinmap_path = drupal_get_path('module', 'pinmap');

    $variables['id'] = self::$i;

    drupal_add_js(url(static::GOOGLE_MAPS_URL, [
      'external' => TRUE,
      'query' => [
        'key' => $variables['info']['api']['key'],
        'libraries' => implode(',', $variables['info']['api']['libraries']),
      ],
    ]), 'external');

    drupal_add_js("$pinmap_path/pinmap.js", [
      'scope' => 'footer',
      // Add main JS to the very bottom of the page.
      'weight' => 1000000,
    ]);

    if (!empty($variables['info']['map']['default_styles_enabled'])) {
      drupal_add_css("$pinmap_path/files/" . str_replace('_', '-', ctools_api_theme_hook(static::class, 'default')) . '.css');
    }

    static::setSettings($variables['info']);
    static::addAssets($variables['theme']);
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    $form['info'] = [
      '#type' => 'horizontal_tabs',
    ];

    // Content type configuration.
    $form['info']['content'] = [
      '#type' => 'horizontal_tab',
      '#title' => t('Source'),
    ];

    $form['info']['content'] += pinmap_form_source_field(t('Source'), TRUE);

    // Google API configuration.
    $form['info']['api'] = [
      '#type' => 'fieldset',
      '#title' => t('API'),
    ];

    $form['info']['api']['key'] = [
      '#type' => 'textfield',
      '#title' => t('Google API key'),
      '#required' => TRUE,
      '#default_value' => Google::getApiKey(),
      '#description' => t('How to create an application with API key? Read here: !link', [
        '!link' => self::externalLink('https://developers.google.com/maps/documentation/javascript/get-api-key'),
      ]),
    ];

    $form['info']['api']['libraries'] = [
      '#type' => 'checkboxes',
      '#title' => t('Libraries'),
      '#description' => t('Detailed description about the libraries available here: !link', [
        '!link' => self::externalLink('https://developers.google.com/maps/documentation/javascript/libraries'),
      ]),
      '#options' => [
        'places' => t('Places'),
        'drawing' => t('Drawing'),
        'geometry' => t('Geometry'),
        'visualization' => t('Visualization'),
      ],
    ];

    $form['info']['api']['libraries']['places'] = [
      '#disabled' => TRUE,
      '#description' => t('Needed to use an autocomplete feature inside of search field.'),
      '#default_value' => 'places',
    ];

    // Search form configuration.
    $form['info']['search'] = [
      '#type' => 'horizontal_tab',
      '#title' => t('Search configuration'),
    ];

    $form['info']['search']['disabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Do not use search'),
    ];

    foreach ([
      'country' => [
        '#type' => 'select',
        '#title' => t('Country to restrict search in'),
        '#options' => pinmap_countries(FALSE),
        '#description' => t('Leave empty to omit restriction.'),
        '#empty_option' => t('- Any -'),
      ],
      'radius' => [
        '#title' => t('Search radius'),
        '#description' => t('Radius where objects will be looked up (in kilometers).'),
        '#default_value' => 5,
        '#element_validate' => ['element_validate_integer_positive'],
      ],
      'limit' => [
        '#title' => t('Results limitation'),
        '#description' => t('Limit results to given number in case where found too much.'),
        '#default_value' => 30,
        '#element_validate' => ['element_validate_integer_positive'],
      ],
      'geolocation' => [
        '#type' => 'checkbox',
        '#title' => t('Add geolocation button'),
        '#description' => t('Use HTML5 geolocation API of browser to seek places around.'),
      ],
      'geolocation_title' => [
        '#title' => t('Title of the geolocation button'),
        '#default_value' => t('Find around you'),
        '#states' => [
          'disabled' => [
            ':input[name*=geolocation]' => ['checked' => FALSE],
          ],
        ],
      ],
      'placeholder' => [
        '#title' => t('Placeholder of the search field'),
        '#default_value' => t('Search pharmacy'),
      ],
      'button' => [
        '#title' => t('Text of the search button'),
        '#default_value' => t('Find now'),
      ],
    ] as $field => $data) {
      $data += [
        '#type' => 'textfield',
      ];

      $form['info']['search'][$field] = array_merge_recursive($data, [
        '#states' => [
          'invisible' => [
            ':input[name*=disabled]' => ['checked' => TRUE],
          ],
        ],
      ]);
    }

    // Google map configuration.
    $form['info']['map'] = [
      '#type' => 'horizontal_tab',
      '#title' => t('Map settings'),
    ];

    $form['info']['map']['height'] = [
      '#type' => 'textfield',
      '#title' => t('Height'),
      '#default_value' => 400,
    ];

    $form['info']['map']['styles'] = [
      '#type' => 'textarea',
      '#title' => t('Map styles'),
      '#description' => t('Generate your own map styles here: !link', [
        '!link' => self::externalLink(self::STYLES_EDITOR_URL),
      ]),
    ];

    $form['info']['map']['animation'] = [
      '#type' => 'select',
      '#title' => t('Animation'),
      '#empty_option' => t('Disabled'),
      '#description' => t('Type of animation to add for markers. More at: !link.', [
        '!link' => self::externalLink('https://developers.google.com/maps/documentation/javascript/markers#animate'),
      ]),
      '#options' => [
        'DROP' => t('Drop'),
        'BOUNCE' => t('Bounce'),
      ],
    ];

    $form['info']['map']['default_styles_enabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Forcibly add default styles'),
      '#description' => t('This option should be used in case when template for map is overridden by theme or other module. Please note that this is not styles for the map, but for preloader, geolocation button and error messages. Recommended to use as an example for development or if you do not have any custom CSS for those elements.'),
      '#default_value' => FALSE,
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormSubmit(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    if (empty($conf['info']['map']['styles'])) {
      $conf['info']['map']['styles'] = '[]';
    }

    $conf['info']['api']['libraries'] = array_filter($conf['info']['api']['libraries']);
  }

  /**
   * Create external link.
   *
   * @param string $url
   *   URL string.
   * @param string|null $text
   *   Link text. URL will be used if not specified.
   *
   * @return string
   *   HTML markup of generated link.
   */
  protected static function externalLink($url, $text = NULL) {
    if (NULL === $text) {
      $text = $url;
    }

    return l($text, $url, ['attributes' => ['target' => '_blank']]);
  }

  /**
   * Set JavaScript settings.
   *
   * @param array $value
   *   Value to set as JavaScript settings.
   */
  protected static function setSettings(array $value) {
    // Set JS settings to the same namespace like this object.
    $parents = explode('\\', __CLASS__);
    $settings = [];

    // Remove "Drupal\".
    unset($parents[0]);
    // Append pane number.
    $parents[] = self::$i;

    drupal_array_set_nested_value($settings, $parents, $value);
    drupal_add_js($settings, 'setting');
  }

  /**
   * Add CSS/JS for customizing the pane.
   *
   * @param string $theme_variant
   *   Theme variant, chosen for displaying.
   */
  protected static function addAssets($theme_variant) {
    foreach (module_implements(self::HOOK_ASSETS) as $module) {
      $module_path = drupal_get_path('module', $module);
      $assets = (array) call_user_func($module . '_' . self::HOOK_ASSETS, $theme_variant);

      foreach (['css', 'js'] as $asset) {
        if (!empty($assets[$asset]) && is_array($assets[$asset])) {
          foreach ($assets[$asset] as $path) {
            $path = "$module_path/$path";

            if (file_exists($path)) {
              call_user_func("drupal_add_{$asset}", $path, [
                'scope' => 'footer',
              ]);
            }
          }
        }
      }
    }
  }

}
