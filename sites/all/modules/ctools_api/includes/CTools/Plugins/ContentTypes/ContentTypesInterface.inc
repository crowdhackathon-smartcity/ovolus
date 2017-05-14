<?php

namespace CTools\Plugins\ContentTypes;

use CTools\Plugins\ConfigurablePluginInterface;
use CTools\Plugins\TemplatablePluginInterface;
use CTools\Plugins\ContextualPluginInterface;
use CTools\PluginContexts;

/**
 * Interface ContentTypesInterface.
 *
 * @package CTools\Plugins\ContentTypes
 */
interface ContentTypesInterface extends TemplatablePluginInterface, ConfigurablePluginInterface, ContextualPluginInterface {

  /**
   * Preprocess variables before passing them to theme engine.
   *
   * @param array $variables
   *   Values from configuration form.
   * @param array $arguments
   *   Content type arguments.
   * @param PluginContexts $contexts
   *   Plugin contexts. @see ContextualPluginInterface::contexts()
   */
  public static function preprocess(array &$variables, array &$arguments, PluginContexts $contexts);

}
