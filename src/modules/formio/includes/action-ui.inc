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

//function formio_action_ui_base_path($plugin) {
//  return $plugin['menu base'] . '/' . $plugin['callback path'];
//}

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
 * @param string $plugin_name
 * @param string $op
 *
 * @return mixed|null|string
 */
function formio_action_ui_switcher($plugin_name, $op) {
  $args = func_get_args();
  $plugin = formio_get_action_ui($plugin_name);
  $handler = formio_action_ui_get_handler($plugin);

  if ($handler) {
    $method = $op . '_action_form';
    if (method_exists($handler, $method)) {
      // replace the first two arguments:
      $args[0] = 'what';
      $args[1] = $_POST;
      return call_user_func_array(array($handler, $method), $args);
    }
  }
  elseif (!empty($plugin['action info'])) {
    return $plugin['action info'];
  }
  else {
    return t('Configuration error. No steps were found.');
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
