<?php

namespace CTools\Plugins;

use CTools\PluginContexts;

/**
 * Interface ConfigurablePluginInterface.
 *
 * @package CTools\Plugins
 * @access private
 */
interface ConfigurablePluginInterface extends PluginInterface {

  /**
   * Configuration form.
   *
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Saved configuration values.
   * @param PluginContexts $contexts
   *   Plugin contexts. @see ContextualPluginInterface::contexts()
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf, PluginContexts $contexts);

  /**
   * Validate form submission.
   *
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $values
   *   Values from the form elements which will be saved.
   * @param PluginContexts $contexts
   *   Plugin contexts. @see ContextualPluginInterface::contexts()
   */
  public static function configurationFormValidate(array &$form, array &$form_state, array &$values, PluginContexts $contexts);

  /**
   * Submission callback for configuration form.
   *
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Saved configuration values.
   * @param PluginContexts $contexts
   *   Plugin contexts. @see ContextualPluginInterface::contexts()
   */
  public static function configurationFormSubmit(array &$form, array &$form_state, array &$conf, PluginContexts $contexts);

  /**
   * Dispatcher for AJAX callbacks.
   *
   * Will be triggered on elements with '#ajax' => TRUE.
   *
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Saved configuration values.
   * @param PluginContexts $contexts
   *   Plugin contexts. @see ContextualPluginInterface::contexts()
   * @param array $commands
   *   Collection of AJAX commands.
   * @param array $element
   *   Triggering element.
   */
  public static function configurationFormElementCallback(array &$form, array &$form_state, array &$conf, PluginContexts $contexts, array &$commands, array $element);

}
