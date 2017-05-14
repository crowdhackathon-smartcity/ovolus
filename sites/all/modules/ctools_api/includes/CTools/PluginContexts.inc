<?php

namespace CTools;

/**
 * Class PluginContexts.
 *
 * @package CTools
 */
class PluginContexts {

  /**
   * List of contexts.
   *
   * @var PluginContext[]
   */
  private $contexts = [];
  /**
   * Raw argument from "ctools" module.
   *
   * @var null|\ctools_context|\ctools_context[]
   */
  private $rawArgument;

  /**
   * PluginContexts constructor.
   *
   * @param \ctools_context[]|\ctools_context|null $contexts
   *   An array of contexts, single context or empty value.
   */
  public function __construct($contexts) {
    $this->rawArgument = $contexts;

    if (!is_array($contexts)) {
      $contexts = [$contexts];
    }

    foreach (array_filter($contexts) as $context) {
      if ($context instanceof \ctools_context) {
        // Change "viewer" by "user" to provide full compatibility of types.
        // For example, when we define context for nodes - we choosing "node"
        // and get the "node", when for users - we choosing "user" and get
        // the "viewer".
        if ('viewer' === $context->keyword) {
          $context->keyword = 'user';
        }

        $this->contexts[] = new PluginContext($context);
      }
    }
  }

  /**
   * Ensure that context is not registered.
   *
   * @param string $argument
   *   Context keyword or argument or "keyword:argument".
   *
   * @return bool
   *   Checking state.
   */
  public function has($argument) {
    return $this->selectBy($argument) !== FALSE;
  }

  /**
   * Get specific context.
   *
   * @param string $argument
   *   Context keyword or argument or "keyword:argument".
   *
   * @return PluginContext
   *   Context object.
   *
   * @throws \InvalidArgumentException
   *   When context does not exists.
   */
  public function get($argument) {
    $context = $this->selectBy($argument);

    if (FALSE === $context) {
      throw new \InvalidArgumentException(t('The "@argument" context is not registered.', [
        '@argument' => $argument,
      ]));
    }

    return $context;
  }

  /**
   * Select context by its properties or their combination.
   *
   * @param string $argument
   *   Context keyword or argument or "keyword:argument".
   *
   * @return PluginContext|bool
   *   Context object or FALSE if it was not found.
   */
  protected function selectBy($argument) {
    foreach ($this->contexts as $context) {
      $context_info = $context->info();

      if (
        in_array($argument, [$context_info->keyword, $context_info->argument]) ||
        $argument === "$context_info->keyword:$context_info->argument"
      ) {
        return $context;
      }
    }

    return FALSE;
  }

  /**
   * Get raw value of constructor argument.
   *
   * @return null|\ctools_context|\ctools_context[]
   *   Raw argument from "ctools" module.
   */
  public function getRawArgument() {
    return $this->rawArgument;
  }

}
