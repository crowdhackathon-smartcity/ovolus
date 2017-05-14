<?php

namespace CTools\Plugins\Access;

use CTools\PluginContexts;
use CTools\Plugins\ContextualPluginInterface;
use CTools\Plugins\ConfigurablePluginInterface;

/**
 * Interface AccessInterface.
 *
 * @package CTools\Plugins\Access
 */
interface AccessInterface extends ConfigurablePluginInterface, ContextualPluginInterface {

  /**
   * Provide summary depending on settings.
   *
   * @param array $conf
   *   Values from configuration form.
   * @param PluginContexts $contexts
   *   Plugin contexts. @see ContextualPluginInterface::contexts()
   *
   * @return string
   *   Summary information.
   */
  public static function summary(array $conf, PluginContexts $contexts);

  /**
   * Check that access granted.
   *
   * @param array $conf
   *   Values from configuration form.
   * @param PluginContexts $contexts
   *   Plugin contexts. @see ContextualPluginInterface::contexts()
   *
   * @return bool
   *   TRUE - access granted, FALSE - not.
   */
  public static function accessible(array $conf, PluginContexts $contexts);

}
