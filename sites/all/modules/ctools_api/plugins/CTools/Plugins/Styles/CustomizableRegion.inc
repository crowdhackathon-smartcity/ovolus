<?php

namespace CTools\Plugins\Styles;

use CTools\PluginContexts;

/**
 * Class CustomizableRegion.
 *
 * @package CTools\Plugins\Styles
 */
class CustomizableRegion extends StylesRegionPlugin {

  use Customizable;

  /**
   * {@inheritdoc}
   */
  public static function name() {
    return t('Customizable region');
  }

  /**
   * {@inheritdoc}
   */
  public static function preprocess(\panels_display $display, array &$settings, array &$variables, PluginContexts $contexts) {
    self::processSettings([], $settings);
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    self::formItem([], $form, $form_state, $conf, [
      'div',
      'main',
      'footer',
      'header',
      'section',
    ]);
  }

}
