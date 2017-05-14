<?php

namespace CTools;

use CTools\Plugins\PluginInterface;
use CTools\Plugins\TemplatablePluginInterface;

/**
 * Class PluginConstructor.
 *
 * @method array info()
 * @method string name()
 * @method string[] dependencies()
 * @method string[] themeVariants()
 * @method \ctools_context[] contexts()
 *
 * @package CTools
 */
class PluginConstructor {

  /**
   * Plugin definition object.
   *
   * @var PluginInterface
   */
  private $object = '';
  /**
   * Interfaces implemented via plugin.
   *
   * @var string[]
   */
  private $implements = [];
  /**
   * CTools plugin type.
   *
   * @var string
   */
  private $type = '';
  /**
   * Plugin definition information.
   *
   * @var array
   */
  private $info = [];

  /**
   * Init new plugin.
   *
   * @param string $object
   *   Plugin definition object.
   * @param string[] $implements
   *   Interfaces implemented via plugin.
   * @param string $type
   *   CTools plugin type.
   */
  public function __construct($object, array $implements, $type) {
    $this->implements = $implements;
    $this->object = $object;
    $this->type = $type;

    $this->info = $this->info();
    $this->info['title'] = $this->name();

    $this
      ->validateDependencies()
      ->callTypeConstructor()
      ->generateThemeHooks()
      ->processContexts();
  }

  /**
   * Call plugin-specific methods.
   *
   * @param string $method
   *   The name of method.
   * @param array $arguments
   *   Method arguments.
   *
   * @return mixed|null
   *   Invocation result or NULL if method does not exist.
   */
  public function __call($method, array $arguments) {
    if (method_exists($this->object, $method)) {
      return call_user_func_array([$this->object, $method], $arguments);
    }

    return NULL;
  }

  /**
   * Get plugin definition information.
   *
   * @return array
   *   Plugin definition information.
   */
  public function getInfo() {
    return $this->info;
  }

  /**
   * Validate plugin dependencies.
   */
  private function validateDependencies() {
    $dependencies = $this->dependencies();

    // Process content type dependencies.
    if (!empty($dependencies) && is_array($dependencies)) {
      $disabled = array_diff_key($dependencies, array_filter(array_map('module_exists', $dependencies)));

      if (!empty($disabled)) {
        // @todo Inform user directly about missing dependencies.
        throw new \RuntimeException(t('The following dependencies of "@title" content type are disabled or does not exists: "@modules".', [
          '@title' => $this->info['title'],
          '@modules' => implode('", "', $disabled),
        ]));
      }
    }

    return $this;
  }

  /**
   * Call plugin type constructor for updating/changing definition information.
   */
  private function callTypeConstructor() {
    $constructor = "ctools_api_$this->type";

    if (!function_exists($constructor)) {
      throw new \InvalidArgumentException(t('The "@type" plugin type is unsupported!', [
        '@type' => $this->type,
      ]));
    }

    /* @param array $information Basic plugin information. */
    /* @param string[] $interfaces Implemented interfaces */
    $constructor($this->info, $this->implements);

    return $this;
  }

  /**
   * Process plugin contexts.
   */
  private function processContexts() {
    $contexts = $this->contexts();

    if (NULL === $contexts) {
      $contexts = [];
    }

    if (!is_array($contexts)) {
      throw new \UnexpectedValueException(t('An array of @type objects must be returned by @method.', [
        '@type' => \ctools_context::class,
        '@method' => "$this->object::contexts()",
      ]));
    }

    if (!empty($contexts) && !empty(array_filter(array_keys($contexts), 'is_string'))) {
      throw new \UnexpectedValueException(t('Numeric array must be returned by @method.', [
        '@method' => "$this->object::contexts()",
      ]));
    }

    $this->info['required context'] = $contexts;

    return $this;
  }

  /**
   * Generate theme hooks.
   */
  private function generateThemeHooks() {
    if (isset($this->implements[TemplatablePluginInterface::class])) {
      $variants = $this->themeVariants();
      // Provide default variant initially.
      $this->info['theme variants'] = ['default' => t('Default')];

      if (is_array($variants)) {
        $this->info['theme variants'] += $variants;
      }

      // Create all theme suggestions.
      foreach ($this->info['theme variants'] as $variant => $title) {
        $this->info['theme hooks'][$variant] = ctools_api_theme_hook($this->object, $variant);
      }
    }

    return $this;
  }

}
