<?php

class FormioPresetUI extends ctools_export_ui {

  var $default_steps;

  /**
   * Implements init().
   *
   * Initialization method for the plugin.
   */
  function init($plugin) {
    $this->name = $plugin['name'];
    $this->default_steps = $plugin['default steps'];
    parent::init($plugin);
  }

  /**
   * The key, or in this case the 'name'.
   *
   * @return string
   *   The key defined by the export schema.
   */
  function key() {
    return $this->plugin['export']['key'];
  }

  /**
   * Implements get_wizard_info().
   *
   * This is where we need to inject 'action_ui' forms if we need to.
   */
  function get_wizard_info(&$form_state) {
    // Include our preset helpers in the form state.
    ctools_form_include($form_state, 'preset-ui', 'formio');
    ctools_form_include($form_state, 'action-ui', 'formio');

    $step = $form_state['step'];
    if (isset($step) && $step == 'settings') {
      // If we are on the 'settings' step then we grab the action ui
      // form we need to inject into the wizard.
      $action = $form_state['item']->action;
      if (!empty($action)) {
        ctools_include('action-ui', 'formio');
        $forms = formio_action_ui_wizard_forms($form_state, $action);

        // This is for parent::get_wizard_info().
        $this->plugin['form info']['order'] += $forms;
        $this->plugin['form info']['add order'] += $forms;

        // This is for us.
        $form_state['object']->plugin['form info']['order'] += $forms;
        $form_state['object']->plugin['form info']['add order'] += $forms;
        $form_state['plugin']['form info']['order'] += $forms;
        $form_state['plugin']['form info']['add order'] += $forms;
      }
    }
    else {
      if (!in_array($step, $this->default_steps)) {
        $action = $form_state['item']->action;
        if (!empty($action)) {
          ctools_include('action-ui', 'formio');
          $forms = formio_action_ui_wizard_forms($form_state, $action);

          // This is for parent::get_wizard_info().
          $this->plugin['form info']['order'] += $forms;
          $this->plugin['form info']['add order'] += $forms;
        }
      }
    }
    return parent::get_wizard_info($form_state);
  }

  /**
   * Implements edit_form().
   *
   * When no 'forms' have been declared for a plugin:
   *   - $plugin['form info']['forms']
   * then this will be called allowing dynamic routing of forms.
   *
   * It will use $plugin['form info']['order'] and build the 'forms' array using
   * 'ctools_export_ui_edit_item_wizard_form' as the 'form id'.
   *
   * You can also define a method such as: edit_form_{$form_state['step']}() if
   * you want to target a specific form. If a method exists it will call it
   * instead of the method.
   *
   * We can assign values to something in $form_state['object'] anytime if we
   * need something to persist. This would probably be done in a submit function
   * e.g., $form_state['object']->something =  $form_state['values']['something'];
   */
  function edit_form(&$form, &$form_state) {
    // @todo: sassify this!
    $form['#attached']['css'][drupal_get_path('module', 'formio') . '/css/formio.css'] = array();
    // The step we are currently on in the wizard.
    $step = $form_state['step'];
    // We are letting the export-ui handle this field.
    if ($step == 'admin') {
      parent::edit_form($form, $form_state);
    }
    elseif (in_array($step, $this->default_steps)) {
      // These are forms specific to this plugin so we know exactly how to call
      // the form functions.
      $function = FORMIO_PRESET . '_' . $step . '_form';
      if (function_exists($function)) {
        $function($form, $form_state);
      }
    }
    else {
      // We can assume this is an action.
      ctools_include('action-ui', 'formio');
      $method = $step . '_form';
      formio_action_ui_get_form($form, $form_state, $form_state['item']->action, $method);
    }
  }

  /**
   * Implements edit_form_submit().
   */
  function edit_form_submit(&$form, &$form_state) {
    $export_key = $this->key();
    $step = $form_state['step'];

    if ($step == 'admin' && !empty($form_state['values'][$export_key])) {
      $form_state['item']->{$export_key} = $form_state['values'][$export_key];
      $form_state['item']->title = $form_state['values']['title'];
    }
    elseif (in_array($step, $this->default_steps)) {
      // These are forms specific to this plugin so we know exactly how to call
      // the form functions.
      $function = FORMIO_PRESET . '_' . $step . '_form_submit';
      if (function_exists($function)) {
        $function($form, $form_state);
      }
    }
    else {
      // We can assume this is an action.
      ctools_include('action-ui', 'formio');
      $method = $step . '_form_submit';
      formio_action_ui_get_form($form, $form_state, $form_state['item']->action, $method);
    }
  }
}
