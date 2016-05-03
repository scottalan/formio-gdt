<?php

function formio_action_ui_process(&$plugin, $info) {
  ctools_include('action-ui', 'formio');

  $title = isset($plugin['title']) ? $plugin['title'] : ucwords(str_replace('_', ' ', $plugin['name']));

  $plugin += array(
    'title' => $title,
    'action forms' => array(),
    'has menu' => TRUE,
  );

  if ($plugin['has menu'] && isset($plugin['menu']) && !empty($plugin['menu'])) {
    $item = array();
    foreach ($plugin['menu'] as $path => $info) {
      $item[$path] = array();
      foreach ($info as $k => $v) {
        $item[$path][$k] = $v;
      }
      $item[$path] += array(
        'file' => $plugin['includes'],
        'file path' => drupal_get_path('module', $plugin['module']) . '/includes',
      );
    }
  }
}

/**
 * Fetch metadata on a specific action plugin.
 *
 * @param string $plugin_name
 *   Name of action plugin file.
 *
 * @return
 *   An array with information about the requested action.
 */
function formio_get_action_ui($plugin_name) {
  ctools_include('plugins');
  return ctools_get_plugins('formio', 'action_ui', $plugin_name);
}

/**
 * Fetch metadata for all action plugins.
 *
 * @return
 *   An array of arrays with information about all available formio action
 *   plugins.
 */
function formio_get_action_uis() {
  ctools_include('plugins');
  return ctools_get_plugins('formio', 'action_ui');
}

/**
 * @todo: Possible switcher for action forms...
 *
 * @param array $form
 * @param array $form_state
 * @param string $action
 * @param string $method
 *
 * @return mixed|null|string
 */
function formio_action_ui_get_form(&$form, &$form_state, $action, $method) {
  // Find the action ui from the action.
  list(, $name) = explode(':', $action);
  // This will give us the action plugin.
  ctools_include('action', 'formio');
  $action_plugin = formio_get_action($name);
  list(, $action_ui) = explode(':', $action_plugin['action ui']);
  // Use the action plugin to get the action ui to use for our wizard.
  $action_ui_plugin = formio_get_action_ui($action_ui);

  $handler = formio_action_ui_get_handler($action_ui_plugin);
  if ($handler) {
    if (method_exists($handler, $method)) {
      call_user_func_array(array($handler, $method), array(&$form, &$form_state));
    }
  }
}

function formio_action_ui_wizard_forms(&$form_state, $action) {
  // Find the action ui from the action.
  list(, $name) = explode(':', $action);
  // This will give us the action plugin.
  ctools_include('action', 'formio');
  $action_plugin = formio_get_action($name);
  list(, $action_ui) = explode(':', $action_plugin['action ui']);
  // Use the action plugin to get the action ui to use for our wizard.
  $action_ui_plugin = formio_get_action_ui($action_ui);

  if (!empty($action_ui_plugin['action forms'])) {
    $form_state['item']->settings['action_ui'] = $action_ui_plugin['action forms'];
    return $action_ui_plugin['action forms'];
  }
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
function formio_action_ui_get_handler($plugin) {
  $cache = &drupal_static(__FUNCTION__, array());
  if (empty($cache[$plugin['name']])) {
    // If a list class is not specified by the plugin, fall back to the
    // default ctools_export_ui plugin instead.
    if (empty($plugin['handler'])) {
      $default = formio_get_action_ui('formio_action_ui');
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
