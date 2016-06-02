<script type="text/javascript">
(function b($, u) {
  if (typeof $.fn.seamless === u) {
    return setTimeout(b, 100);
  }
  $(function () {
    $('#formio-form-<?php print $formio_name; ?>').seamless({fallback: false}).receive(function (event, submission) {
    });
  });
})(jQuery);
</script>
<iframe id="formio-form-<?php print $formio_name; ?>" style="width:100%;border:none;" height="250px"
  src="/<?php print $module_path; ?>/view/dist/view/index.html#/<?php print $project_hash; ?>/form/<?php print $formio_id; ?>?action=<?php print $callback; ?>&iframe=1<?php print $theme; ?>&header=0&name=<?php print $formio_name; ?>&token=<?php print $csrf_token; ?>">
</iframe>
