(function ($) {

  Drupal.behaviors.mazeblock_themeNavigation = {
    attach: function (context, settings) {
      var $context = $(context);

      $('#mobile-menu', context).bind('click', function(e) {
        e.preventDefault();
        $('body').toggleClass('mobile-menu-active');
      });

      $('.l-mobile-overlay, .mobile-menu-close', context).bind('click', function(e) {
        e.preventDefault();
        $('body').removeClass('mobile-menu-active');
      });
    }
  }

  $(document).bind('mazeblock_theme:appear', function() {
    $('.main-navigation').clone().appendTo('.l-mobile-menu');
  });

  // Default behavior.
  Drupal.behaviors.mazeblock_theme = {
    attach: function (context, settings) {
      var $context = $(context);

      // $(window).load(function() {
      //   setTimeout(function(){
      //     $(".custom-filter-group").each(function() {
      //       var wrap = $(this);
      //       var element_children = wrap.find(".sk-item-list-option__count");
      //       $(element_children).each(function() {
      //         if($(this).text()  == '0') {
      //           $(this).parent('.sk-item-list__item').parent('.sk-item-list').parent('.sk-panel__content').parent('.sk-panel').hide();
      //         }
      //       });
      //     });
      //     $(".custom-filter-group").each(function() {
      //       var wrap = $(this);
      //       if(wrap.children(".sk-panel:visible").length == 0) {
      //         wrap.hide();
      //       } else {
      //         wrap.show();
      //       }
      //     });
      //   }, 1000)
      // });

      // Replace broken book images
      $('.views-field-field-product-image img').on('error', function() {
        $(this).attr('src', '/sites/all/themes/mazeblock_theme/images/BookNotPictured.jpg');
      });

      // Sticky cart on the checkout bage
      if ($(window).width() > 1140) {
        $('#commerce-checkout-form-checkout .cart-contents-wrapper').sticky({
          topSpacing: 20,
          bottomSpacing: 655
        });
      }

      $('.continue-review').bind('click', function(event) {
        $('html, body').animate({
            scrollTop: $("#summary-tab").offset().top - 100
        }, 500);
      });

      //Checkout - show invoice details on change
      $( '.shipping-recalculation-processed' ).bind('change', function() {
        if (this.value == 'invoice') {
          $('.group-invoice').show();
        } else if (this.value == 'receipt'){
          $('.group-invoice').hide();
        }
      });

      //Front page category open/close for responsive
      $('#block-mazeblock-react-search-front-categories .block__title', context).bind('click', function(e) {
        $('#block-mazeblock-react-search-front-categories').toggleClass('open');
      });

      // Mobile Menu clone from main menu
      var menu = $('#block-system-main-menu').clone();
      $('.l-mobile-menu').append(menu);

      // Mobile Sidebar.
      $('.mobile-sidebar-btn', context).bind('click', function(e) {
        e.preventDefault();
        $('.l-sidebar').toggleClass('active');
      });

      $('.details-tabs li a').click(function(e) {
        e.preventDefault();
        $('.tabs-wrapper .active-tab').removeClass('active-tab');
        $('.details-tabs li a').removeClass('active');
        $('.tabs-wrapper #' + $(this).attr('class')).addClass('active-tab');
        $(this).addClass('active');
      });

      // Messages.
      $('div.messages', context).find('button.close').bind('click', function(e) {
        e.preventDefault();
        $(this).closest('div.messages').slideUp(300, function() {
          $(this).remove();
        });
      });

      // Add filled class to input elements.
      $('input.form-text', context).bind('focus', function() {
        $(this).closest('.form-item').addClass('form-animate')
      }).bind('keyup', function() {
        if (this.value !== '') {
          $(this).closest('.form-item').addClass('filled');
        }
      }).bind('change blur', function() {
        if (this.value === '') {
          $(this).closest('.form-item').removeClass('filled');
        }
        else {
          $(this).closest('.form-item').addClass('filled');
        }
      }).trigger('blur');
    }
  }

  // Show dropdown.
  function showdropdown() {
    $(this).addClass('hover');
    $(this).children('a').addClass('hover');
    clearInterval(this.timer);
  }

  // Hide dropdown.
  function hidedropdown() {
    $(this).removeClass('hover');
    $(this).children('a').removeClass('hover');
    clearInterval(this.timer);
  }

  Function.prototype.mybind = function(object) {
    var method = this;
    return function() {
      method.apply(object, arguments);
    };
  };

})(jQuery);
