// Using a closure to map jQuery to $.
(function ($, Drupal) {

  Drupal.behaviors.formioRender = {
    attach: function (context, settings) {
      if (settings.formioRender) {
        $(document).ready(function (arg1, arg2) {
          // Do something when form renders.
          var stop = true;
        });
      }
    },
    detach: function (context) {}
  };

  Drupal.behaviors.formioReload = {
    attach: function (context, settings) {
      if (settings.formioReload) {
        $(document).ready(function (response) {
          // Do something when form renders.
          var stop = true;
        });
      }
    },
    detach: function (context) {}
  };

  if (typeof Drupal.ajax !== 'undefined') {
    Drupal.ajax.prototype.commands.formioReload = function (ajax, response) {
      window.location.reload();
    };
  }

  $("#reload-preset").bind("click", function () {
    var text = $("#edit-formio-form option:selected").text();
    var id = $("#edit-formio-form").val();
  });

}(jQuery, Drupal));
