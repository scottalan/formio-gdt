<?php
/**
 * @file
 * The base class for formio submissions.
 */

class FormioSubmission {

  var $name = '';

  function init($plugin) {
//    ctools_include('submission', 'formio');
    $this->plugin = $plugin;
    $this->name = $plugin['name'];


  }

}
