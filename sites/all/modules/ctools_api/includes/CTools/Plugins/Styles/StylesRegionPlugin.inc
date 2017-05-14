<?php

namespace CTools\Plugins\Styles;

use CTools\PluginContexts;

/**
 * Class StylesRegionPlugin.
 *
 * @package CTools\Plugins\Styles
 */
abstract class StylesRegionPlugin implements StylesRegionInterface {

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
  public static function preprocess(\panels_display $display, array &$settings, array &$variables, PluginContexts $contexts) {

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
