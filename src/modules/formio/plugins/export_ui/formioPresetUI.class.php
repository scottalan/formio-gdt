<?php

class FormioPresetUI extends ctools_export_ui {

  /**
   * Implements init().
   *
   * Initialization method for the plugin.
   */
  function init($plugin) {
    $this->name = $plugin['name'];
    parent::init($plugin);
  }

  /**
   * Implements get_wizard_info().
   */
  function get_wizard_info(&$form_state) {
    ctools_include('action-ui', 'formio');
    $action = $form_state['item']->action;
    if (!empty($action)) {
      list($module, $plugin_name, $type) = explode(':', $action);

      $action_form = formio_action_ui_switcher($plugin_name, 'edit');

      if (!$action_form) {
        ctools_include($type, $module);
        $plugin = formio_get_action_ui($plugin_name);
        $action_form = $plugin['action form'];
      }

      if (isset($action_form)) {
        // Go ahead and store the steps in the preset.
        $form_state['item']->settings['steps'] = $action_form;

        foreach ($action_form as $key => $info) {
          $this->plugin['form info']['add order'][$key] = $info['title'];
          $this->plugin['form info']['order'][$key] = $info['title'];
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
    $export_key = $this->plugin['export']['key'];
    $step = $form_state['step'];
    if ($step == 'admin') {
      parent::edit_form($form, $form_state);
    }
    else {
      $function = $form_state['plugin'][$export_key] . '_' . str_replace(':', '_', $step) . '_form';
      if (function_exists($function)) {
        $function($form, $form_state);
      }
      elseif (!empty($form_state['item']->action)) {
        $action = $form_state['item']->action;
        list($module, $plugin_name, $type) = explode(':', $action);
        ctools_include($type, $module);
        $plugin = formio_get_action_ui($plugin_name);
        $action_form = $plugin['action form'];

        foreach ($action_form as $key => $info) {
          if ($key == $step) {
            if (function_exists($info['form'])) {
              $info['form']($form, $form_state);
            }
          }
        }
      }
    }
  }

  /**
   * Implements edit_form_submit().
   */
  function edit_form_submit(&$form, &$form_state) {
    $export_key = $this->plugin['export']['key'];
    $step = $form_state['step'];

    if ($step == 'admin' && !empty($form_state['values'][$export_key])) {
      $form_state['item']->{$export_key} = $form_state['values'][$export_key];
    }
    else {
      $function = $form_state['plugin'][$export_key] . '_' . str_replace(':', '_', $step) . '_form_submit';
      if (function_exists($function)) {
        $function($form, $form_state);
      }
      elseif (!empty($form_state['item']->action)) {
        $action = $form_state['item']->action;
        list($module, $plugin_name, $file) = explode(':', $action);
        ctools_include($file, $module);
        $plugin = formio_get_action_ui($plugin_name);
        $action_form = $plugin['action form'];

        foreach ($action_form as $key => $info) {
          if ($key == $step) {
            $function = $info['form'] . '_submit';
            if (function_exists($function)) {
              $function($form, $form_state);
            }
          }
        }
      }
    }
  }
}
