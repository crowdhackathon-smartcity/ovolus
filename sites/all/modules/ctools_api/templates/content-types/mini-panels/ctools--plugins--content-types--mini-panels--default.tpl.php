<?php

/**
 * @file
 * Default theme variant for Entity Reference content type plugin.
 *
 * @see \CTools\Plugins\ContentTypes\MiniPanels
 *
 * @var array $content
 */
?>
<?php foreach ($content['panels'] as $name => $panel) : ?>
  <?php print $panel['content']; ?>
<?php endforeach; ?>
