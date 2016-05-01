<?php
/**
 * @file
 */

function formio_submission_process(&$plugin, $info) {
  // Unless the plugin specifically defines a 'menu callback' we
  // set it to FALSE.
  $plugin += array(
    'menu callback' => FALSE,
    'title' => $plugin['name'],
    'menu item' => array(),
    'redirect' => array(),
    'action info' => array(),
    'access' => 'formio forms submit',
  );
  if (isset($plugin['menu callback']) && !empty($plugin['menu callback'])) {
    $plugin['menu callback'] = FALSE;
  }

  // By default we also show a block title but this can be overwritten
  if (!isset($plugin['action info']) || empty($plugin['action info'])) {
    drupal_set_message(t('The plugin definition of @plugin uses actions, but none are defined.', array('@plugin' => $plugin['name'])), 'error');
    return;
  }

  if (!isset($plugin['action include'])) {
    $plugin['action include'] = TRUE;
  }

  // Add a block view function
  if (!isset($plugin['action info']['callback'])) {
    $plugin['view'] = $plugin['module'] . '_' . $plugin['name'] . '_' . $plugin['action info']['action'] . '_submission';
  }
}

function formio_submission_submit($plugin_name) {

  // Load the $plugin information
  $plugin = ctools_get_export_ui($plugin_name);
  $handler = ctools_export_ui_get_handler($plugin);

  if ($handler) {
    $method = $op . '_page';
    if (method_exists($handler, $method)) {
      // replace the first two arguments:
      $args[0] = $js;
      $args[1] = $_POST;
      return call_user_func_array(array($handler, $method), $args);
    }
  }
  else {
    return t('Configuration error. No handler found.');
  }
}

function formio_submission_get_handler($plugin) {
  $cache = &drupal_static(__FUNCTION__, array());
  if (empty($cache[$plugin['name']])) {
    // If a list class is not specified by the plugin, fall back to the
    // default ctools_export_ui plugin instead.
    if (empty($plugin['handler'])) {
      $default = ctools_get_export_ui('ctools_export_ui');
      $class = ctools_plugin_get_class($default, 'handler');
    }
    else {
      $class = ctools_plugin_get_class($plugin, 'handler');
    }

    if ($class) {
      $cache[$plugin['name']] = new $class();
      $cache[$plugin['name']]->init($plugin);
    }
  }
  return !empty($cache[$plugin['name']]) ? $cache[$plugin['name']] : FALSE;
}
