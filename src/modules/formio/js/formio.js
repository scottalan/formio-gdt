// Using a closure to map jQuery to $.
(function ($, Drupal) {

  Drupal.behaviors.formioRender = {
    attach: function (context, settings) {
      if (settings.formioRender) {
        $(document).ready(function () {
          // Do something when form renders.
        });
      }
    },
    detach: function (context) {}
  };

}(jQuery, Drupal));
