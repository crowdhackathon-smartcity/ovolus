<?php

/**
 * @file
 * Theming guide for templatable plugin.
 *
 * @var string $type
 *   Type of plugin.
 * @var string $name
 *   Plugin name.
 * @var string[] $templates
 *   List of templates for plugin.
 * @var string $subdirectory
 *   Subdirectory for plugin templates.
 */

$path = sprintf('%s/templates/ctools/%s/%s', drupal_get_path('theme', $GLOBALS['conf']['theme_default']), strtr($type, '_', '-'), $subdirectory);
?>
<?php
  print t('Open a <span id="@id">theming guide</span> to get the information about creating the assets.', [
    '@id' => 'theming_guide_trigger',
  ]);
?>

<div id="theming_guide" title="<?php print t('Theming guide for @plugin', ['@plugin' => $name]); ?>" data-width="666" hidden>
  <p>
    <?php
      print t('You have to create the following templates inside of <b>@directory</b> directory:', [
        '@directory' => $path,
      ]);
    ?>
  </p>

  <ol>
    <?php foreach ($templates as $template => $variant): ?>
      <li>
        <?php
          print t('<i>@template</i> for <b>@variant</b> variant.', [
            '@template' => "$template.tpl.php",
            '@variant' => $variant,
          ]);
        ?>
      </li>
    <?php endforeach; ?>
  </ol>

  <p>
    <?php print t('Remember that template assets (CSS/JS) could be loaded automatically if they are located in a correct place. To provide a possibility of dynamic loading you have to place the assets in the following directories:'); ?>
  </p>

  <ol>
    <?php foreach ($templates as $template => $variant): ?>
      <li>
        <?php
          print t('For <b>@variant</b> variant:', [
            '@variant' => $variant,
          ]);
        ?>
        <ul>
          <?php foreach (['css', 'js'] as $asset): ?>
            <li>
              <?php print str_replace('templates', $asset, $path) . "/$template.$asset"; ?>
            </li>
          <?php endforeach; ?>
        </ul>
      </li>
    <?php endforeach; ?>
  </ol>
</div>
