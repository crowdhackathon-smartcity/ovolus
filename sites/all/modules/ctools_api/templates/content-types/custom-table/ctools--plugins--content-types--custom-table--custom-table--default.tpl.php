<?php

/**
 * @file
 * Default theme variant for Entity Reference content type plugin.
 *
 * @see \CTools\Plugins\ContentTypes\CustomTable\CustomTable
 *
 * @var array $content
 */
?>
<?php if (!empty($content['head'])): ?>
  <table class="tablelist">
    <?php if (!empty($content['override_title_text'])): ?>
      <caption>
        <?php print $content['override_title_text']; ?>
      </caption>
    <?php endif; ?>

    <thead>
      <tr>
        <?php foreach ($content['head'] as $head): ?>
          <th class="align-<?php print $head['align']; ?>">
            <?php print $head['head']; ?>
          </th>
        <?php endforeach; ?>
      </tr>
    </thead>

    <tbody>
      <?php foreach ($content['body'] as $i => $row): ?>
        <tr class="<?php print $i % 2 ? 'odd' : 'even'; ?>">
          <?php foreach ($row as $key => $value): ?>
            <?php if ('weight' !== $key): ?>
              <td class="align-<?php print $content['head'][$key]['align']; ?>">
                <?php print $value; ?>
              </td>
            <?php endif; ?>
          <?php endforeach; ?>
        </tr>
      <?php endforeach; ?>
    </tbody>
  </table>
<?php endif; ?>
