<?php

class formioPresetUI extends ctools_export_ui {

  public function init($plugin) {
    ctools_include('export');
    $this->plugin = $plugin;
  }

  public function edit_form(&$form, &$form_state) {

    parent::edit_form($form, $form_state);

    $this->formio_form_presets_form($form, $form_state);


  }

  public function edit_form_submit(&$form, &$form_state) {

    $this->formio_form_presets_form_submit($form, $form_state);
  }

  public function hook_menu(&$items) {
    // Use this to override or extend menus for the preset.
    parent::hook_menu($items);
  }

  /**
   * Forms used for presets.
   *
   * @param bool $reset
   * @return array
   */
  public function formio_get_forms($reset = FALSE) {
    static $drupal_static_fast;
    if (!isset($drupal_static_fast)) {
      $drupal_static_fast = &drupal_static(__FUNCTION__);
    }
    $formio = &$drupal_static_fast;

    if ($reset || !isset($formio)) {
      $formio = new FormioRequest();
      $formio
        ->endpoint('form')
        ->params(array('type' => 'form'))
        ->request();
    }

    return $formio;
  }

  private function formio_get_preset_actions() {
    $options = module_invoke_all('io_preset_action_info');
    $options = array('_none' => t('Choose an Action...')) + $options;
    return $options;
  }

  /**
   * Preset form used to collect information about actions Formio will perform.
   *
   * @return array
   *   Form containing preset selections.
   */
  public function formio_form_presets_form(&$form, &$form_state) {

    $formio = $this->formio_get_forms();
    $forms = $formio->fetchOptions();

    if (isset($form_state['values']) && !empty($form_state['values']['formio_form'])) {
      $form_state['item']->_id = $form_state['values']['formio_form'] == '_none' ? NULL : $form_state['values']['formio_form'];
      $form_state['item']->api_path = $forms[$form_state['item']->_id]['path'];
      $form_state['item']->formio = $forms[$form_state['item']->_id];
    }

    if (!empty($forms)) {

      $options = array();
      $options['_none'] = t('Choose your form...');
      foreach ($forms as $id => $data) {
        $options[$id] = $data['title'];
      }

      // Placeholder for rendered formio form.
      $form['formio_render'] = array(
        '#type' => 'fieldset',
        '#title' => t('Formio'),
        '#states' => array(
          'invisible' => array(
            ':input[name="formio_form"]' => array('value' => '_none'),
          ),
        ),
      );
      $form['formio_render']['formio_display'] = array(
        '#markup' => '<div id="formio-form-render"></div>',
      );

      $form['formio_presets'] = array(
        '#type' => 'fieldset',
        '#title' => t('Preset Options'),
      );
      $form['formio_presets']['formio_form'] = array(
        '#title' => t('Form IOs'),
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => isset($form_state['item']->_id) ? $form_state['item']->_id : '',
        '#prefix' => '<div id="formio-presets-form">',
        '#suffix' => '</div>',
        '#ajax' => array(
          'callback' => array($this, 'formio_form_presets_ajax'),
          'wrapper' => 'formio-form-render',
          'path' => 'system/formio_ajax',
          'method' => 'replace',
        ),
      );

      $this->formio_form_action_form($form, $form_state);
    }
  }

  /**
   * Ajax callback.
   *
   * Renders the Form IO form that is selected.
   *
   * @param array $form
   *   Form that triggered the callback.
   * @param array $form_state
   *   State of the form.
   *
   * @return array
   *   Ajax commands.
   */
  public static function formio_form_presets_ajax($form, $form_state) {
    $commands = array();
    // The form to target via the api.
    $path =  $form_state['item']->api_path;
    // Render the form.
    $block = module_invoke('formio', 'block_view', 'formio_form', $path);
    // Render the html in the Formio fieldset.
    $commands[] = ajax_command_html('#formio-form-render', $block['content']);

    // Get the form we want to append.
    $get_form = drupal_get_form('formio_form_action');
    // Make sure we don't duplicate the form.
    $commands[] = ajax_command_remove('#formio-form-action');
    $commands[] = ajax_command_append('#formio-presets-form', render($get_form));

    // Passed to: 'Drupal.ajax.prototype.commands'.
    $commands[] = array(
      'command' => 'formioRender',
      'form' => $form_state['item']->api_path,
      'api' => variable_get('formio_project_url', NULL),
      'target' => '#formio-form-render',
    );

    // Return commands to the system to be executed.
    return array('#type' => 'ajax', '#commands' => $commands);
  }

  /**
   * ACTION FORM.
   */
  public function formio_form_action_form(&$form, &$form_state) {
    $form['formio_presets']['formio_action'] = array(
      '#title' => t('Action'),
      '#type' => 'select',
      '#options' => $this->formio_get_preset_actions(),
      '#prefix' => '<div id="formio-action">',
      '#suffix' => '</div>',
      '#states' => array(
        'invisible' => array(
          ':input[name="formio_form"]' => array('value' => '_none'),
        ),
      ),
      '#ajax' => array(
        'callback' => array($this, 'formio_form_action_ajax'),
        'wrapper' => 'formio-presets-form',
        'path' => 'system/formio_ajax',
        'method' => 'append',
      ),
    );
  }

  /**
   * Ajax callback.
   *
   * Actions can be anything therefore we need a way to generate the form element
   * based on the option that was selected. We use the 'key' of the option to
   * to build the hook we want to invoke.
   *
   * @param array $form
   *   Form that triggered the callback.
   * @param array $form_state
   *   State of the form.
   */
  public static function formio_form_action_ajax($form, $form_state, $args) {
    $action = $form_state['values']['formio_action'];
    if ($action != '_none') {
      $hook = 'io_preset_action_render__' . $action;
      foreach (module_implements($hook) as $module) {
        $function = $module . '_' . $hook;
        // Expects an array of ajax commands.
        $commands = $function($form, $form_state);
        return $commands;
      }
    }
  }

  /**
   * Override the submit handler for our preset.
   *
   * @param $form
   * @param $form_state
   */
  public function formio_form_presets_form_submit($form, &$form_state) {
    // Transfer data from the form to the $item based upon schema values.
    $schema = ctools_export_get_schema($this->plugin['schema']);

    // Set all the default items and any others that match the key.
    foreach (array_keys($schema['fields']) as $key) {
      if(isset($form_state['values'][$key])) {
        $form_state['item']->{$key} = $form_state['values'][$key];
      }
    }

    // Set our custom values.
    $form_state['item']->_id = $form_state['values']['formio_form'];
    $form_state['item']->action = $form_state['values']['formio_action'];
    $form_state['item']->data = array();
  }
}
