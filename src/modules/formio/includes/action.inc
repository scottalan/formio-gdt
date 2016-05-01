<?php

function formio_actions_process(&$plugin, $info) {

  $title = isset($plugin['title']) ? $plugin['title'] : ucwords(str_replace('_', ' ', $plugin['name']));

  $plugin += array(
    'title' => $title,
    'action' => 'default',
    'has menu' => FALSE,
    'callback' => 'formio_submission_handler',
  );
}

/**
 * Fetch metadata on a specific action plugin.
 *
 * @param string $module
 *   Name of the module.
 * @param string $type
 *   Type of plugin, such as 'action_ui'.
 * @param string $file
 *   Name of action plugin file.
 *
 * @return
 *   An array with information about the requested action.
 */
function formio_get_action($plugin_name) {
  ctools_include('plugins');
  return ctools_get_plugins('formio', 'actions', $plugin_name);
}

/**
 * Fetch metadata for all action plugins.
 *
 * @return
 *   An array of arrays with information about all available formio action
 *   plugins.
 */
function formio_get_actions() {
  ctools_include('plugins');
  return ctools_get_plugins('formio', 'actions');
}

/**
 * Gets a class if available. Similar to ctools_export_ui_get_handler().
 *
 * @param array $plugin
 *   The plugin as returned from formio_get_action_type().
 *
 * @return object|bool
 *   A class object or False.
 */
function formio_actions_get_handler($plugin) {
  $cache = &drupal_static(__FUNCTION__, array());
  if (empty($cache[$plugin['name']])) {
    // If a list class is not specified by the plugin, fall back to the
    // default ctools_export_ui plugin instead.
    if (empty($plugin['handler'])) {
      $default = formio_get_action('formio_actions');
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

/**
 * General submission handler called when a form.io form is submitted.
 *
 * @param string $plugin_name
 *   The name of the plugin defining the callback action.
 * @param array $args
 *   Arguments that will be passed to the action.
 */
function formio_submission_handler($plugin_name, $args = NULL) {
  $plugin = formio_get_action($plugin_name);
  $handler = formio_actions_get_handler($plugin);
  if ($handler) {
    $handler->action($plugin, $args);
  }
}
