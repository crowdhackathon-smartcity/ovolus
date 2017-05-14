<?php

namespace CTools\Plugins\Styles;

use CTools\PluginContexts;

/**
 * Class StylesPanePlugin.
 *
 * @package CTools\Plugins\Styles
 */
abstract class StylesPanePlugin implements StylesPaneInterface {

  /**
   * {@inheritdoc}
   */
  public static function info() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public static function themeVariants() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public static function dependencies() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public static function contexts() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public static function preprocess(\stdClass $content, array &$settings, array &$variables, PluginContexts $contexts) {

  }

  /**
   * {@inheritdoc}
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {

  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormValidate(array &$form, array &$form_state, array &$values, PluginContexts $contexts) {

  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormSubmit(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {

  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormElementCallback(array &$form, array &$form_state, array &$conf, PluginContexts $contexts, array &$commands, array $element) {

  }

}
