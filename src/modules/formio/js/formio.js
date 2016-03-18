// Using the closure to map jQuery to $.
(function ($, Drupal) {

  $(function() {
    angular.module('formApp', ['formio']);
    angular.bootstrap(document, ['formApp']);
  });

  Drupal.ajax.prototype.commands.doSomething = function(ajax, response) {
    // The parent of the new element
    var $target = $('#formio-form-display');

    // The new element to be added
    //var $formio = $('<formio src="\'https://ukpryfgoedtaslz.form.io/' + form + '\'"></formio>');
    var $formio = $('<formio src="\'' + response.api + '/' + response.form + '\'"></formio>');

    angular.element($target).injector().invoke(function($compile) {
      var $scope = angular.element($target).scope();
      $target.html($compile($formio)($scope));
      // Finally, refresh the watch expressions in the new element
      $scope.$apply();
    });
  };

}(jQuery, Drupal));
