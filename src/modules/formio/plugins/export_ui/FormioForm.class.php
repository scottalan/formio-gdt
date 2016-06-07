<?php

class FormioForm extends ctools_export_ui {

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
   * {@inheritdoc}
   */
  function get_page_title($op, $item = NULL) {
    if (empty($this->plugin['strings']['title'][$op])) {
      return;
    }

    // Replace %title that might be there with the exportable title.
    $title = $this->plugin['strings']['title'][$op];
    if (!empty($item)) {
      $key = $this->plugin['export']['admin_title'];
      $title = (str_replace('%title', check_plain($item->{$key}), $title));
    }

    return $title;
  }

}
