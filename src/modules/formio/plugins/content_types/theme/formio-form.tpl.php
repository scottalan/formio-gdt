<script type="text/javascript">
(function b($, u) {
  if (typeof $.fn.seamless === u) {
    return setTimeout(b, 100);
  }
  $(function () {
    $('#formio-form-<?php print $formio_name; ?>').seamless({fallback: false}).receive(function (event, submission) {
      <?php if (isset($redirect)): ?>
        window.location = <?php print $redirect; ?>;
      <?php endif; ?>
    });
  });
})(jQuery);
</script>
<iframe id="formio-form-<?php print $formio_name; ?>" style="width:100%;border:none;" height="250px"
  src="/<?php print $module_path; ?>/view/src/view/index.html#/<?php print $project_hash; ?>/form/<?php print $formio_id; ?>?action=<?php print $callback; ?>&iframe=1<?php print $theme; ?>&header=0<?php print $name; ?>&token=<?php print $token; ?>">
</iframe>
