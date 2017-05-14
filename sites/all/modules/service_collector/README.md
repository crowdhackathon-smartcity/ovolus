# Service Collector

Module aims to simplify routines of collecting and modifying the data via `module_invoke()` and `drupal_alter()`.

## Example

```php
use Drupal\pinmap\Searcher\SearcherInterface;
use Drupal\pinmap\Searcher\Coordinates;
use Drupal\pinmap\Searcher\Name;

/**
 * Implements hook_pinmap_searchers().
 */
function pinmap_pinmap_searchers() {
  $searchers = [];

  $searchers['coordinates'] = Coordinates::class;
  $searchers['name'] = Name::class;

  return $searchers;
}

/**
 * Implements hook_pinmap_searchers_alter().
 */
function pinmap_pinmap_searchers_alter(array &$searchers) {
  // This entry will be removed due to it incorrectness.
  $searchers['coordinates'] = 'Coordinates::class';
}

/**
 * Returns list of registered searchers.
 *
 * @return SearcherInterface[]
 *   List of registered searchers.
 */
function pinmap_searchers() {
  return service_collector(__FUNCTION__, function ($name, $class) {
    if (!is_subclass_of($class, SearcherInterface::class, TRUE)) {
      throw new \InvalidArgumentException(sprintf('Searcher "%s" must implement the "%s" interface', $class, SearcherInterface::class));
    }
  });
}
```
