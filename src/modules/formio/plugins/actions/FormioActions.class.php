<?php

interface FormioActionsInterface {

  function init($plugin);
  function action($plugin, $preset_name, $export);
}
class FormioActions implements FormioActionsInterface {

  // The plugin.
  var $plugin;
  // The name of the plugin
  var $name;
  // A human readable name of the plugin.
  var $title;

  function init($plugin) {
    $this->plugin = $plugin;
    $this->name = $plugin['name'];
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
   *
   * @param array $plugin
   * @param string $preset_name
   * @param object $export
   */
  function action($plugin, $preset_name, $export) {}
}
