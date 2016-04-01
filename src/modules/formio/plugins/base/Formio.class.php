<?php

interface Formio {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
  public function getId();

  /**
   * Returns the action to be performed when the form is submitted.
   *
   * @return string
   */
  public function getAction();

}

abstract class FormioDefaults {

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
  protected function setHeader() {
    return array('x-jwt-token' => $this->getToken());
  }

  protected function loadAssets() {
    drupal_add_js('https://cdnjs.cloudflare.com/ajax/libs/angular.js/1.5.0/angular.js', array(
        'type' => 'external',
        'group' => JS_THEME,
        'weight' => -1,
      )
    );
    drupal_add_js(drupal_get_path('module', 'formio') . '/js/formio-complete.min.js', array(
        'group' => JS_THEME,
        'weight' => 2,
      )
    );
    drupal_add_js(drupal_get_path('module', 'formio') . '/js/formio.js', array(
      'group' => JS_THEME,
      'weight' => 10,
    ));
    drupal_add_css(drupal_get_path('module', 'formio') . '/css/formio-complete.min.css');
    drupal_add_css(drupal_get_path('module', 'formio') . '/css/formio.css');
  }

}

class FormioRequest extends FormioDefaults{

  public $endpoint;
  public $path;
  public $params;
  public $operation;
  public $response_type;
  public $status;
  public $response;
  public $project;
  public $forms = array();

  /**
   * Formio constructor.
   * @param string $operation
   */
  public function __construct($operation = 'GET') {
    $this->project = trim(variable_get('formio_project_url', NULL));
    $this->header = array('headers' => $this->setHeader());
    $this->operation = $operation;
    $this->params = array();
    $this->loadAssets();
  }

  /**
   * @return array
   */
  public function getRequestOptions() {
    return array(
      'headers' => $this->setHeader(),
      'method' => $this->getOp(),
      'data' => $this->getParams(),
    );
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
  public function getOp() {
    return $this->operation;
  }

  /**
   *
   */
  public function response() {
    $this->response_type = 'json';
  }

  /**
   * @param array $params
   * @return $this
   */
  public function params(Array $params) {
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
   * @param $data
   */
  public function decode($data) {

  }

  /**
   * @return $this|bool
   */
  public function request() {
    $request = drupal_http_request($this->getPath(), $this->getRequestOptions());
    $this->setStatus($request->code);

    if ($this->getStatus() != 200) {
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
   * @param $key
   * @return array
   */
  public function fetchKeyed($key) {
    $results = array();
    foreach ($this->forms as $delta => $form) {
      $results[$form[$key]] = $form;
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
