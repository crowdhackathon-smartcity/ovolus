<?php

/**
 * @file
 * Default theme implementation to display a single Drupal page.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $secondary_menu_heading: The title of the menu used by the secondary links.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['branding']: Items for the branding region.
 * - $page['header']: Items for the header region.
 * - $page['navigation']: Items for the navigation region.
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['highlighted']: Items for the highlighted content region.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['sidebar_second']: Items for the second sidebar.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see omega_preprocess_page()
 */

if (!empty($page['sidebar_first']) || !empty($page['sidebar_first'])) {
  $content_class = 'with-sidebar';
}
else {
  $content_class = 'no-sidebar';
}

?>
<div class="search-overlay"></div>
<div class="l-page">
  <div class="l-mobile-overlay"></div>
  <nav class="l-mobile-menu">
    <a href="#" class="mobile-menu-close">
      <span class="line"></span>
      <span class="line"></span>
    </a>
  </nav>
  <header class="l-header">
    <div class="l-header-inner container">
      <div class="header-left">
        <?php print render($page['branding']); ?>
      </div>

      <div class="l-branding">
        <?php if ($logo): ?>
          <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" class="site-logo"><img width="233" height="59" src="/sites/all/themes/ovolus_theme/logo.png" alt="<?php print t('Home'); ?>"/></a>
        <?php endif; ?>

        <h3 class="site-slogan"><?php print $site_slogan ?></h3>
      </div>

      <div class="header-right">
        <?php print render($page['header']); ?>
      </div>

      <a href="#" id="mobile-menu">
        <span class="line"></span>
        <span class="line"></span>
        <span class="line"></span>
      </a>
    </div>
  </header>

  <?php if ($page['navigation']): ?>
    <div class="main-navigation">
      <div class="container">
        <?php print render($page['navigation']); ?>
      </div>
    </div>
  <?php endif; ?>

  <?php if ($breadcrumb && FALSE): ?>
    <div class="l-breadcrumb">
      <div class="container">
        <?php print $breadcrumb; ?>
      </div>
    </div>
  <?php endif; ?>

  <div class="highlighted-region">
    <?php if($front_page): ?>
      <div class="l-region l-region--highlighted">
        <div id="block-views-hero-block-block" class="block block--views hero-banner block--views-hero-block-block">
          <div class="block__content">
            <div class="view view-hero-block view-id-hero_block view-display-id-block view-dom-id-dd31e7f234928d1ae3ee64b091b49cd7">
              <div class="view-content">
                <div class="views-row views-row-1 views-row-odd views-row-first views-row-last">
                  <div class="slide-wrapper with-filter" style="background-image: url(/sites/all/themes/ovolus_theme/images/pexels-photo-380283.jpg)">
                    <div class="slide-content container">
                      <h2>Κάντε την πόλη σας πιο έξυπνη.<br>
                         Κάντε τους δημότες σας πιο χαρούμενους.
                      </h2>
                      <h3>Η λύση πληρωμών και συμολαίων που παρέχει ασφάλεια, οικονομία και ταχύτητα.</h3>
                      <a href="/user/register" class="sign-up-button">Sign Up For Free</a>
                      <a href="#" class="solutions">Explore Our Solutions</a>    
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    <?php endif; ?>
  </div>

  <div class="l-main">
    <div class="l-content container">
      <a id="main-content"></a>
      <?php if($messages): ?>
        <div class="l-messages">
          <?php print $messages; ?>
        </div>
      <?php endif;?>

      <?php if ($tabs): ?>
        <?php print render($tabs); ?>
      <?php endif; ?>

      <?php if ($page['sidebar_first']): ?>
        <div class="l-sidebar">
          <?php print render($page['sidebar_first']); ?>
        </div>
      <?php endif; ?>

      <div class="content-wrapper <?php print $content_class; ?>">
        <?php //print render($title_prefix); ?>
        <?php //if ($title): ?>
          <!-- <h1><?php //print $title; ?></h1> -->
        <?php //endif; ?>
        <?php //print render($title_suffix); ?>

        <?php print render($page['help']); ?>

        <?php if ($action_links): ?>
          <ul class="action-links"><?php print render($action_links); ?></ul>
        <?php endif; ?>
        <?php print render($page['content']); ?>
        <?php if ($page['sidebar_second']): ?>
     </div>
     <div class="l-sidebar">
          <?php print render($page['sidebar_second']); ?>
        </div>
      <?php endif; ?>
      </div>
  </div>

<div class="suggested">
    <div class="container">
      <?php print render($page['suggested']); ?>
    </div>
</div>

  <?php if ($page['newsletter']): ?>
    <div class="newsletter">
        <div class="container">
          <?php print render($page['newsletter']); ?>
        </div>
    </div>
  <?php endif; ?>

  <footer class="l-footer">
    <div class="container">
      <?php print render($page['footer']); ?>
    </div>
    <div class="footer-bottom">
      <div class="footer-bottom-inner container">
        <div class="copyrights">© <?php print date('Y'); ?> <?php print $site_name; ?></div>
        <div class="footer-mazeblock">Site by <a href="http://mazeblock.com" target="_blank">Mazeblock</a></div>
      </div>
    </div>
  </footer>
</div>