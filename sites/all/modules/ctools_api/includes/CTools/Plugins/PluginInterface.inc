<?php

namespace CTools\Plugins;

/**
 * Interface PluginInterface.
 *
 * @package CTools\Plugins
 * @access private
 */
interface PluginInterface {

  /**
   * Get plugin name.
   *
   * @return string
   *   Human-readable name.
   */
  public static function name();

  /**
   * Additional plugin information. Could be empty.
   *
   * @return string[]
   *   For "content_types":
   *     - 'category' => <string>,
   *     - 'top level' => <bool>,
   *   For "access":
   *     - 'description' => <string>,
   *   For "styles":
   *     - 'description' => <string>,
   */
  public static function info();

  /**
   * Plugin dependencies (Drupal modules).
   *
   * @return string[]
   *   Module machine names.
   */
  public static function dependencies();

}
