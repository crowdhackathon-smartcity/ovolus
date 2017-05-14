<?php

/**
 * Implements theme_menu_tree().
 */
function ovolus_theme_menu_tree__main_menu($variables) {
  return '<ul class="menu">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree().
 */
function ovolus_theme_menu_tree__menu_footer_menu($variables) {
  return '<ul class="menu">' . $variables['tree'] . '</ul>';
}

function ovolus_theme_menu_alter(&$items) {
  $items['taxonomy/term/%taxonomy_term']['page callback'] = 'mazeblock_react_search_taxonomy_term_page';
}

/**
 * Implements hook_preprocess().
 */
function ovolus_theme_preprocess_block(&$variables) {
  if ($variables['block']->delta == 'menu-footer-menu') {
    $main_tree = menu_tree('main-menu');
    $tree = '';
    foreach (element_children($main_tree) as $key) {
      if ($main_tree[$key]['#original_link']['module'] == 'taxonomy_menu') {
        $main_tree[$key]['#attributes']['class'][] = 'visible-xs';
        $tree .= drupal_render($main_tree[$key]);
      }
    }
    $start = strpos($variables['content'], '<li');
    $variables['content'] = substr($variables['content'], 0, $start) . $tree . substr($variables['content'], $start);
  }

  if ($variables['block']->module == 'locale' and $variables['block']->delta == 'language_content') {
    global $language;
    $variables['content'] = '<ul class="menu language-menu"><li><span>€ ' . $language->native . '</span>' . $variables['content'] . '</li></ul>';
  }
}

/**
 * Implements theme_menu_link().
 */
function ovolus_theme_menu_link(array $variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if ($element['#below']) {
    $sub_menu = drupal_render($element['#below']);
  }

  if (isset($element['#original_link']['link_path'])) {
    $element['#attributes']['class'][] = drupal_html_class($element['#original_link']['link_path']);
  }

  if ($element['#href'] != '<nolink>') {
    $element['#localized_optioovolus_theme']['html'] = TRUE;
    $element['#title'] = '<span>' . $element['#title'] . '</span>';
  }

  if (isset($element['#localized_optioovolus_theme']) && is_array($element['#localized_optioovolus_theme'])) {
    $output = l($element['#title'], $element['#href'], $element['#localized_optioovolus_theme']);
  }
  else {
    $output = l($element['#title'], $element['#href'], array('html', TRUE));
  }
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Implements hook_preprocess_textfield().
 */
function ovolus_theme_preprocess_textfield(&$variables) {
  if (!empty($variables['element']['#title']) and !isset($variables['element']['#attributes']['placeholder'])) {
    $variables['element']['#attributes']['placeholder'] = $variables['element']['#title'];
  }
}

/**
 * Implements theme_status_messages().
 */
function ovolus_theme_status_messages($variables) {
  $display = $variables['display'];
  $output = '';

  $status_heading = array(
    'status' => t('Status message'),
    'error' => t('Error message'),
    'warning' => t('Warning message'),
  );
  foreach (drupal_get_messages($display) as $type => $messages) {
    $output .= '<div class="messages messages--' . $type . '">';
    $output .= '<button type="button" class="close"><span>×</span></button>';
    if (!empty($status_heading[$type])) {
      $output .= '<h2 class="element-invisible">' . $status_heading[$type] . "</h2>";
    }
    if (count($messages) > 1) {
      $output .= '<ul>';
      foreach ($messages as $message) {
        $output .= '  <li>' . $message . '</li>';
      }
      $output .= '</ul>';
    }
    else {
      $output .= $messages[0];
    }
    $output .= '</div>';
  }

  return $output;
}
