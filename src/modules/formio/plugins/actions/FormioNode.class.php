<?php
/**
 * @file
 * Defines the FormioNode class.
 *
 * This class creates node entities with submission data.
 */

class FormioNode {

  /**
   * Initialize the FormioNode class.
   *
   * @param array $plugin
   */
  function init($plugin) {
    global $user;
    $this->uid = $user->uid;
  }


}
