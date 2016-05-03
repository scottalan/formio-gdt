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
 * Fetches a specific formio action plugin.
 *
 * @param string $plugin_name
 *   Name of the plugin.
 *
 * @return
 *   An array with information about the requested action.
 */
function formio_get_action($plugin_name) {
  ctools_include('plugins');
  return ctools_get_plugins('formio', 'actions', $plugin_name);
}

/**
 * Fetches all action plugins.
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
 * Gets an action plugin from a module.
 *
 * @param string $module
 * @param string $action
 *
 * @return mixed
 */
function formio_get_module_action($module, $action) {
  ctools_include('plugins');
  return ctools_get_plugins($module, 'actions', $action);
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
 * Default submission handler.
 *
 * This is called when a form.io form is submitted. It calls the action()
 * method of the plugin that defined it.
 *
 * @param string $plugin_name
 *   The name of the plugin defining the callback action.
 * @param string $preset_name
 *   The name of the preset as stored in the database.
 */
function formio_submission_handler($plugin_name, $preset_name) {
  // Load the preset export from the database.
  ctools_include('export');
  $export = ctools_export_crud_load(FORMIO_PRESET, $preset_name);
  $key = $export->action;
  list($module, $action) = explode(':', $key);

  $plugin = formio_get_module_action($module, $action);
  $handler = formio_actions_get_handler($plugin);
  if ($handler) {
    // We don't need to pass by reference to just call the method direct.
    $handler->action($plugin, $preset_name, $export);
  }
}
