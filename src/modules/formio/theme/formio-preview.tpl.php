<?php
/**
 * @file
 * Template for a Form.io form preview.
 */
?>
<div class="formio-preview-wrapper <?php print $classes; ?>"<?php print $attributes; ?>>
  <div class="formio-preview-title-wrapper">
    <?php if (!empty($add_link)): ?>
      <div class="formio-preview-title"><?php print $add_link; ?></div>
    <?php endif; ?>
    <span class="formio-preview-title-inner"><?php print $title; ?></span>
  </div>
  <div class="formio-preview-inner">
    <?php if (is_string($preview)): ?>
      <?php print $preview; ?>
    <?php else: ?>
      <?php render($preview); ?>
    <?php endif; ?>
  </div>
</div>
