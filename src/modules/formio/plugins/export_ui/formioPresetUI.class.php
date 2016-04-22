<?php

class FormioPresetUI extends ctools_export_ui {

  function init($plugin) {
    ctools_include('preset', 'formio');
    parent::init($plugin);
  }

  function edit_wizard_next(&$form_state) {
    $this->store_preset_info($form_state);
    parent::edit_wizard_next($form_state);
  }

  function edit_wizard_finish(&$form_state) {
    parent::edit_wizard_finish($form_state);
  }

  function hook_menu(&$items) {
    // Use this to override or extend menus for the preset.
    parent::hook_menu($items);
  }

  function get_wizard_info(&$form_state) {
    $form_info = parent::get_wizard_info($form_state);
    return $form_info;
  }

  function store_preset_info(&$form_state) {
    if ($form_state['step'] != 'import') {
      // Set our custom values.
      if (isset($form_state['values']['formio_form'])) {
        $form_state['item']->id = $form_state['values']['formio_form'];
      }
      if (isset($form_state['values']['formio_action']) && $form_state['values']['formio_action'] != '_none') {
        $form_state['item']->action = $form_state['values']['formio_action'];
      }
      if (isset($form_state['values']['settings']) && !empty($form_state['values']['settings'])) {
        $form_state['item']->settings = $form_state['values']['settings'];
      }
    }
  }

}
