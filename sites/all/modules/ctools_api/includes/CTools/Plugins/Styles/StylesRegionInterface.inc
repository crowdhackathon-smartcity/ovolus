<?php

namespace CTools\Plugins\Styles;

use CTools\PluginContexts;

/**
 * Interface StylesRegionInterface.
 *
 * @package CTools\Plugins\Styles
 */
interface StylesRegionInterface extends StylesInterface {

  /**
   * Preprocess variables before passing them to theme engine.
   *
   * @param \panels_display $display
   *   Values from configuration form.
   * @param array $settings
   *   Values from configuration form.
   * @param array $variables
   *   All available variables (including "display" and "settings").
   * @param PluginContexts $contexts
   *   Plugin contexts. @see ContextualPluginInterface::contexts()
   */
  public static function preprocess(\panels_display $display, array &$settings, array &$variables, PluginContexts $contexts);

}
