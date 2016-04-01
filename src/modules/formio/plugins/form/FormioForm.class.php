<?php
/**
 * Base class for the Formio Form plugin.
 */

interface FormioFormInterface {

  public function type();
  public function getId();
  public function getTitle();
  public function getName();
  public function getPath();
  public function getComponents();

//  // Hooks specific to FormIO presets.
//  public function presetForm(&$form, &$form_state);
//  // public function presetFormValidate(&$form, &$form_state);
//  public function presetFormSubmit(&$form, &$form_state);
//
//  // Hooks specific to FormIO forms and submissions.
//  public function accessSubmisson($type, $role);
//  public function accessForm($type, $role);
//
//  // Hooks specific to FormIO submissions.
//  public function createSubmission();
//  public function readSubmission();
//  public function updateSubmission();
//  public function deleteSubmission();
//  public function loadSubmission();
}

abstract class FormioForm implements FormioFormInterface {

  public $plugin = NULL;
  public $formId = '';
  public $title = '';
  public $name = '';
  public $path = '';
  public $access = TRUE;
  public $endpoint = '';
  public $params = array();
  public $operation = '';
  public $response_type = '';
  public $status;
  public $response;
  public $project;
  public $forms = array();


  public $components = array();
  public $componentType = '';
  public $componentKey = '';
  public $componentLabel = '';
  public $componentPlaceholder = '';
  public $componentInputType = '';
  public $componentInput = TRUE;

  public $tags = array();

  function __construct($plugin) {
    $this->plugin = $plugin;
    $this->name = $plugin['name'];
  }
}

class Formio2 extends FormioForm {

  function __construct($plugin, $operation) {
    parent::__construct($plugin);
    $this->project = variable_get('formio_project_url', NULL);
    $this->header = array('headers' => formio_header());
    $this->operation = $operation;
    $this->params = array();
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

  /**
   * Protected Methods.
   */

  /**
   * @return string
   */
  protected function getToken() {
    // If this doesn't work go and get a new one. (May Expire).
    $token = file_get_contents(drupal_get_path('module', 'formio') . '/token');
    if (!isset($token) || empty($token)) {
      drupal_set_message(t('You need a token file!'), 'error');
    }
    return file_get_contents(drupal_get_path('module', 'formio') . '/token');
  }

  /**
   * @return array
   */
  protected function setHeader() {
    return array('x-jwt-token' => $this->getToken());
  }
}
