<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="X-UA-Compatible" content="IE=edge" />
  <style type="text/css">
    <?php print file_get_contents(drupal_get_path('theme', 'ovolus_theme') . '/css/ovolus_theme.mail.css'); ?>
  </style>
</head>
<body>
  <table width="100%" cellpadding="0" cellspacing="0" border="0" id="background-table">
    <tbody>
      <tr>
        <td align="center" bgcolor="#e9e9e9"><table class="w640" style="margin:0 10px;" width="640" cellpadding="0" cellspacing="0" border="0">
            <tbody>
              <tr>
                <td class="w640" width="640" height="20"></td>
              </tr>
              <tr>
                <td class="w640" width="640"><table id="top-bar" class="w640" width="640" cellpadding="0" cellspacing="0" border="0">
                    <tbody>
                      <tr>
                        <td>
                          <a target="_blank" href="<?php print url('<front>', array('absolute' => TRUE)); ?>" title="<?php print t('Home'); ?>" rel="home" class="site-logo"><img width="200" src="/<?php print $theme_path; ?>/logo.png" alt="<?php print t('Home'); ?>" border="0"/></a>
                        </td>
                      </tr>
                    </tbody>
                  </table></td>
              </tr>
              <tr>
                <td class="w640" width="640" height="20"></td>
              </tr>
              <tr>
                <td class="w640" width="640" height="20" bgcolor="#ffffff"></td>
              </tr>
              <tr id="simple-content-row">
                <td class="w640" width="640" bgcolor="#ffffff"><table class="w640" width="640" cellpadding="0" cellspacing="0" border="0">
                    <tbody>
                      <tr>
                        <td class="w30" width="30"></td>
                        <td class="w580" width="580">
                          <?php print $body; ?>
                        </td>
                        <td class="w30" width="30"></td>
                      </tr>
                      <tr><td>&nbsp;</td></tr>
                    </tbody>
                  </table></td>
              </tr>
              <tr>
                <td class="w640" width="640" height="20"></td>
              </tr>
            </tbody>
          </table></td>
      </tr>
    </tbody>
  </table>
</body>
</html>
