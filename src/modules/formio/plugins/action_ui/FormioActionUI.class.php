<?php

interface FormioActionUIInterface {

  function init($plugin);
}
class FormioActionUI implements FormioActionUIInterface {
  var $plugin;
  var $name;
  var $action_forms = array();

  function init($plugin) {
    $this->plugin = $plugin;
    $this->name = $plugin['name'];
    $this->action_forms = $plugin['action forms'];
  }
}
