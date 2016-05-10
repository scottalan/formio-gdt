<?php

/**
 * Makes a rest call to Form.io.
 *
 * @param string $path
 *   The api path.
 * @param array $params
 *   Parameters added to the request.
 * @param string $method
 *   Method used for the request.
 *
 * @return object
 *   The result of the request.
 *
 */
function formio_rest($path, $params = array(), $method = 'GET') {
  $request = new Formio();
  $request
    ->method($method)
    ->endpoint($path)
    ->setParams($params)
    ->load();

  return $request;
}

/**
 * Return a flattened list of components.
 */
function formio_flatten_components($components, $includeAll = FALSE) {
  $flattened = array();

  foreach($components as $component) {
    $hasColumns = isset($component['columns']);
    $hasRows = isset($component['rows']);
    $hasComps = isset($component['components']);
    $isTree = isset($component['tree']) && $component['tree'];
    $noRecurse = FALSE;
    if ($includeAll || $isTree || (!$hasColumns && !$hasRows && !$hasComps)) {
      $flattened[$component['key']] = $component;
    }
    else {
      if ($hasColumns) {
        foreach($component['columns'] as $column) {
          array_merge($flattened, formio_flatten_components($column['components'], $includeAll));
        }
      }
      elseif ($hasRows && is_array($component['rows'])) {
        foreach($component['rows'] as $row) {
          array_merge($flattened, formio_flatten_components($row['components'], $includeAll));
        }
      }
      elseif ($hasComps) {
        array_merge($flattened, formio_flatten_components($component['components'], $includeAll));
      }
    }
  }

  return $flattened;
}

function formio_passthrough($module, $type, &$items, $append = NULL) {
  $files = file_scan_directory(drupal_get_path('module', $module) . '/includes', '/\.' . $type . '\.inc$/', array('key' => 'name'));
  foreach ($files as $file) {
    require_once DRUPAL_ROOT . '/' . $file->uri;
    list($tool) = explode('.', $file->name, 2);

    $function = $module . '_' . str_replace('-', '_', $tool) . '_' . str_replace('-', '_', $type);
    $function = isset($append) ? $function . '_' . $append : $function;
    if (function_exists($function)) {
      $function($items);
    }
  }
}

/**
 * Get all forms from form.io.
 *
 * @param bool $reset
 * @return array
 */
function formio_get_formio_forms($reset = FALSE) {
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast = &drupal_static(__FUNCTION__);
  }
  $formio = &$drupal_static_fast;

  if ($reset || !isset($formio)) {
    $formio = formio_rest('form', array('type' => 'form', 'limit' => 99999));
  }

  return $formio->result;
}

/**
 * Get all forms that are presets.
 *
 * @param bool $reset
 *   Allows flushing the static cache.
 *
 * @return array
 *   All preset forms.
 */
function formio_get_all_exports($reset = FALSE) {
  ctools_include('export');
  return ctools_export_crud_load_all(FORMIO_PRESET, $reset);
}

/**
 * Get multiple forms that are presets.
 *
 * @param array $names
 *   An array of preset names.
 *
 * @return array
 *   Multiple presets.
 */
function formio_get_exports(array $names) {
  ctools_include('export');
  return ctools_export_crud_load_multiple(FORMIO_PRESET, $names);
}

/**
 * Get a single preset form.
 *
 * @param string $name
 *   The name of the preset.
 *
 * @return array
 *   A single preset.
 *
 *
 * @todo: What now? If the {formio_form} is updated it breaks the display.
 */
function formio_get_export($export) {
  if (is_numeric($export)) {
    return formio_get_export_by_pid($export);
  }
  ctools_include('export');
  return ctools_export_crud_load(FORMIO_PRESET, $export);
}

/**
 * Remove an item from the object cache.
 */
function formio_clear_form_cache($name) {
  ctools_include('object-cache');
  ctools_object_cache_clear(FORMIO_PRESET, $name);
}

interface FormioFormControllerInterface {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
//  public function getId();

  /**
   * Returns the action to be performed when the form is submitted.
   *
   * @return string
   */
//  public function getAction();

  public function load();

}

class Formio implements FormioFormControllerInterface{

  public $endpoint;
  public $path;
  public $header;
  public $params;
  public $body;
  public $method = 'GET';
  public $response_type;
  public $status;
  public $response;
  public $project;
  public $result = array();

  /**
   * @return string
   */
  private function apiKey() {
    // If this doesn't work go and get a new one. (May Expire).
    $apikey = trim(variable_get('formio_project_api_key', FALSE));
    if (!isset($apikey) || empty($apikey)) {
      drupal_set_message(t('Looks like you need an API Key. Visit !formio to create one now.', array('!formio' => l('Form.io', 'https://form.io'))), 'warning');
    }
    return $apikey;
  }

  /**
   * @return array
   */
  protected function setApiKey() {
    return array('x-token' => $this->apiKey());
  }

  protected function setPostHeader() {
    return array('Content-Type' => 'application/x-www-form-urlencoded');
  }

  /**
   * Formio constructor.
   */
  public function __construct() {
    // Get the project url from settings and strip whitespace and the
    // end forward slash if it exists.
    $this->project = trim(variable_get('formio_project_url', NULL), '\ \/');
    $this->header = array('headers' => $this->setApiKey());
    $this->params = array();
  }

  /**
   * The api portion of the url.
   *
   * @param string $url
   * @return $this
   */
  public function endpoint($url) {
    $this->endpoint = $url;
    return $this;
  }

  /**
   * The type of interaction we want with the form.
   *
   * @param string $method
   *   This would be either: get, post, put, delete.
   *
   * @return string
   */
  public function method($method) {
    $this->method = $method;
    return $this;
  }

  /**
   *
   */
  public function response() {
    $this->response_type = 'json';
  }

  public function setBody($obj) {
    $this->body = $obj;
    return $this;
  }

  public function getBody() {
    return $this->body;
  }

  /**
   * @param array $params
   * @return $this
   */
  public function setParams(Array $params) {
    foreach ($params as $k => $v) {
      $this->params[] = urlencode($k) . '=' . urlencode($v);
    }
    return $this;
  }

  /**
   * @return null|string
   */
  public function getParams() {
    if (!empty($this->params)) {
      return '?' . implode('&', $this->params);
    }
    return NULL;
  }

  /**
   * @return string
   */
  public function getPath() {
    return $this->project . '/' . $this->endpoint . $this->getParams();
  }

  /**
   * @param $status
   */
  public function setStatus($status) {
    $this->status = $status;
  }

  /**
   * @return mixed
   */
  public function getStatus() {
    return $this->status;
  }

  /**
   * @return array
   */
  public function getRequestOptions() {
    return array(
      'headers' => $this->setApiKey(),
      'method' => $this->method,
    );
  }

  public function getData() {
    $options = $this->getRequestOptions();
    switch ($options['method']) {
      case 'GET':
        $options += array('data' => $this->getParams());
        break;

      case 'POST':
        $options['headers'] += array('Content-Type' => 'application/json');
        $options += array('data' => $this->body);
        break;
    }
    return $options;

  }

  /**
   * @return $this|bool
   */
  public function load() {
    $request = drupal_http_request($this->getPath(), $this->getData());
    $this->setStatus($request->code);

    if ($request->code != 200) {
      $error = t('Status message: %status, Code: %code, Error: %error', array(
        '%status' => $request->status,
        '%code' => $request->code,
        '%error' => $request->error,
      ));
      drupal_set_message($error, 'error');
      return FALSE;
    }

    $this->result = drupal_json_decode($request->data);

    return $this;

  }

  /**
   * Fetches the form or forms keyed by a specific element.
   *
   * @param string $key
   *   The element you would like the result array(s) keyed by.
   *
   * @return array
   */
  public function fetchKeyed($key) {
    $results = array();
    if (isset($this->result[$key])) {
      $results = $this;
    }
    else {
      foreach ($this->result as $delta => $form) {
        $results[$form[$key]] = $form;
      }
    }
    return $results;
  }

  /**
   * @return array
   */
  public function fetchOptions() {
    $results = array();
    foreach ($this->result as $delta => $form) {
      $results[$form['_id']] = array(
        'path' => $form['path'],
        'title' => $form['title'],
      );
    }
    return $results;
  }

  /**
   * @return array
   */
  public function fetchComponents() {
    $results = array();
    $components = $this->result['components'];
    foreach ($components as $component) {
      if ($component['type'] != 'button') {
        $results[] = array(
          'key' => $component['key'],
          'type' => $component['type'],
          'label' => $component['label'],
          'validate' => $component['validate'],
        );
      }
    }
    return $results;
  }

  /**
   *
   */
  public function selectDisplay() {
    $display = array();
    foreach($this->result as $key => $form) {
      $display[$form['name']] = $form['title'];
    }
    $this->result = $display;
  }

}
