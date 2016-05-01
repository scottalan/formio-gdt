<?php
/**
 * @file
 * Defines a menu items for action plugins.
 */

function formio_action_menu(&$items) {
  ctools_include('action', 'formio');

  // If a menu rebuild is triggered because of module enable/disable,
  // this might be out of date. Reset the cache.
  ctools_include('plugins');
  ctools_get_plugins_reset();

  foreach (formio_get_actions() as $plugin) {
    // We also need to make sure that the module hasn't been disabled. During
    // the disable process, the module's plugins still still appear.
    if ($plugin['has menu'] && module_exists($plugin['module'])) {
      $handler = formio_actions_get_handler($plugin);
      if ($handler) {
        $handler->hook_menu($items);
      }
    }
  }
}
