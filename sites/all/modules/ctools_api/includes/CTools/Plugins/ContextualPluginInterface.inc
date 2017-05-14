<?php

namespace CTools\Plugins;

/**
 * Interface ContextualPluginInterface.
 *
 * @package CTools\Plugins
 * @access private
 */
interface ContextualPluginInterface {

  /**
   * Define plugin contexts.
   *
   * @see \CTools\PluginConstructor::processContexts()
   *
   * @return \ctools_context[]
   *   An associative array of contexts. Each context
   *   will be available by a key in this array.
   */
  public static function contexts();

}
