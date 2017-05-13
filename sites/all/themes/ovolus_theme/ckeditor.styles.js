/*
Copyright (c) 2003-2013, CKSource - Frederico Knabben. All rights reserved.
For licensing, see LICENSE.html or http://ckeditor.com/license
*/

/*
 * This file is used/requested by the 'Styles' button.
 * The 'Styles' button is not enabled by default in DrupalFull and DrupalFiltered toolbars.
 */
if(typeof(CKEDITOR) !== 'undefined') {
  CKEDITOR.addStylesSet( 'drupal', [
    {name: 'Paragraph', element: 'p'},
    {name: 'Heading 1', element: 'h1'},
    {name: 'Heading 2', element: 'h2'},
    {name: 'Heading 3', element: 'h3'},
    {name: 'Small', element: 'small'},
    {name: 'Small with Separator', element: 'small', 'attributes': {'class': 'separator'}}
  ]);
}
