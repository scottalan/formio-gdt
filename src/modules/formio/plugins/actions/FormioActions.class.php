<?php

interface FormioActionsInterface {

  function init($plugin);
  function action($plugin);
}
class FormioActions implements FormioActionsInterface {
  var $plugin;
  var $name;
  var $title;
  var $action;

  function init($plugin) {
    $this->plugin = $plugin;
    $this->name = $plugin['name'];
    $this->action = $plugin['action'];
    $this->title = isset($plugin['title']) ? $plugin['title'] : ucwords(str_replace('_', ' ', $plugin['name']));
  }

  /**
   * Used to define a menu item used by the form for a callback.
   *
   * @param array $items
   *   Menu items.
   */
  function hook_menu(&$items) {
    foreach ($items as &$item) {
      // Add menu item defaults.
      $item += array(
        'file' => 'action.inc',
        'file path' => drupal_get_path('module', 'formio') . '/includes',
      );
    }
  }

  /**
   * Used to implement the action.
   */
  function action($plugin) {}
}
