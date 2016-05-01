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
  public $forms = array();

  /**
   * @return string
   */
  private function getToken() {
    // If this doesn't work go and get a new one. (May Expire).
    $token = trim(variable_get('formio_project_api_key', FALSE));
    if (!isset($token) || empty($token)) {
      drupal_set_message(t('You need a token file!'), 'error');
    }
    return $token;
  }

  /**
   * @return array
   */
  protected function setTokenHeader() {
    return array('x-jwt-token' => $this->getToken());
  }

  protected function setPostHeader() {
    return array('Content-Type' => 'application/x-www-form-urlencoded');
  }

  /**
   * Formio constructor.
   */
  public function __construct() {
    $this->project = trim(variable_get('formio_project_url', NULL), '\ \/');
    $this->header = array('headers' => $this->setTokenHeader());
    $this->params = array();
  }

  /**
   * @param $url
   * @return $this
   */
  public function endpoint($url) {
    $this->endpoint = $url;
    return $this;
  }

  /**
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
      'headers' => $this->setTokenHeader(),
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

    $this->forms = drupal_json_decode($request->data);

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
    if (isset($this->forms[$key])) {
      $results = $this;
    }
    else {
      foreach ($this->forms as $delta => $form) {
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
    foreach ($this->forms as $delta => $form) {
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
    $components = $this->forms['components'];
    foreach ($components as $component) {
      if ($component['type'] != 'button') {
        $results[] = array(
          'key' => $component['key'],
          'type' => $component['type'],
          'label' => $component['label'],
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
    foreach($this->forms as $key => $form) {
      $display[$form['name']] = $form['title'];
    }
    $this->forms = $display;
  }

}
