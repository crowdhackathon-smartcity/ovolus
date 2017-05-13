CKEDITOR.addTemplates('default', {
  imagesPath: Drupal.settings.basePath + 'sites/all/themes/ns/images/',
  templates : [{
    title: 'Banner with caption text',
    image: 'ck-template-1.png',
    description: 'Banner with caption text.',
    html: '<div contenteditable="false" class="image-banner"><a href="/"><span class="image-placeholder"><img src="' + Drupal.settings.basePath + 'sites/all/themes/ns/images/ck-empty-image.png' + '" alt="" width="550" height="650" /></span><span class="caption" contenteditable="true"><span class="caption-title"><small>Small Title</small>Big Title<small class="separator">Small with separator</small></span></span></a></div>'
  }]
});
