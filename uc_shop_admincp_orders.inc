<?php
function uc_shop_admincp_orders($sql = NULL, $args = NULL, $search = FALSE) {
  global $_domain;
  $domain_id = $_domain['domain_id'];
  $header = array(
    array('data' => t('Actions')),
    array(
      'data' => t('Order ID'),
      'field' => 'o.order_id',
      'sort' => 'desc',
    ),
    array('data' => t('Customer')),
    array(
      'data' => t('Total'),
      'align' => 'center',
      'field' => 'o.order_total',
    ),
    array(
      'data' => t('Purchase date'),
      'align' => 'center',
      'field' => 'o.created',
    ),
    array(
      'data' => t('Status'),
      'field' => 'os.title',
    ),
//    array(
//      'data' => '分店',
//      'field' => 'dm.sitename',
//    ),
  );

  if (is_null($sql)) {
    $args = array();
    $show_status = 1;
    // $sql = 'SELECT o.order_id, o.uid, o.billing_first_name, o.billing_last_name, o.order_total, o.order_status, o.created, os.title '
    //      . 'FROM {uc_orders} o LEFT JOIN {uc_order_statuses} os ON o.order_status = os.order_status_id ';
     $sql = 'SELECT o.order_id, o.uid, o.billing_first_name, o.billing_last_name, o.order_total, o.order_status, o.created, os.title, dm.sitename '
          . 'FROM {uc_orders} o LEFT JOIN {uc_order_statuses} os ON o.order_status = os.order_status_id LEFT JOIN {uc_domain_orders} AS do ON o.order_id = do.order_id
    LEFT JOIN {domain} AS dm ON dm.domain_id = do.domain_id ';
        

    if (arg(3) == 'sort' && !is_null(arg(4))) {
      $_SESSION['sort_status'] = arg(4);
      $args[] = arg(4);
    }
    $where = '';
/*    if (!isset($_SESSION['sort_status']) || $_SESSION['sort_status'] != 'all') {
      if (!empty($_SESSION['sort_status']) && is_string($_SESSION['sort_status'])) {
        $where = "WHERE o.order_status = '%s'";
        $args[] = $_SESSION['sort_status'];
      }
      else {
        $where = 'WHERE o.order_status IN ' . uc_order_status_list('general', TRUE);
      }
    }*/
    
    // modify.
    if (!isset($_SESSION['sort_status']) || $_SESSION['sort_status'] != 'all') {
      if (!empty($_SESSION['sort_status']) && is_string($_SESSION['sort_status'])) {
        $where = "WHERE o.order_status = '%s'";
        $args[] = $_SESSION['sort_status'];
      }
      else {
        $where = 'WHERE o.order_status IN ' . uc_order_status_list('general', TRUE);
      }
      $where .= ' AND dm.domain_id = %s';
    } else {
      $where = 'WHERE dm.domain_id = %s';
    }
    $args[] = $domain_id;
    // modified.
    
    $sql .= $where;
  }

  // Need to include tablesort_sql() when the SQL command is passed as an
  // argument AND when we build the SQL command here in this function.
  $sql .= tablesort_sql($header);

  $address = variable_get('uc_customer_list_address', 'billing');
  if ($address == 'shipping') {
    $sql = str_replace('billing', 'delivery', $sql);
  }
  else {
    $address = 'billing';
  }

  $context = array(
    'revision' => 'themed-original', 
    'type' => 'amount',
  );

  $rows = array();

  $result = pager_query($sql, variable_get('uc_order_number_displayed', 30), 0, NULL, $args);
  while ($order = db_fetch_object($result)) {
    if ($address == 'shipping') {
      $order_name = $order->delivery_first_name . ' ' . $order->delivery_last_name;
    }
    else {
      $order_name = $order->billing_first_name . ' ' . $order->billing_last_name;
    }
    if (trim($order_name) == '') {
      if ($order->uid !== 0) {
        $account = db_result(db_query("SELECT name FROM {users} WHERE uid = %d", $order->uid));
      }
      if (empty($account)) {
        $order_name = t('User: none');
      }
      else {
        $order_name = t('User: !name', array('!name' => $account));
      }
    }

    $rows[] = array(
      'data' => array(
        array('data' => uc_order_actions($order, TRUE)),
        array('data' => $order->order_id),
        array('data' => check_plain($order_name)),
        array(
          'data' => uc_price($order->order_total, $context),
          'align' => 'right',
        ),
        array(
          'data' => format_date($order->created, 'custom', variable_get('uc_date_format_default', 'm/d/Y')),
          'align' => 'center',
        ),
        array('data' => $order->title),
//        array('data' => $order->sitename),
      ), 
      'id' => 'order-' . $order->order_id,
    );
  }

  drupal_add_js(array(
    'ucURL' => array(
      'adminOrders' => url('shop/admincp/orders'),
    ),
  ), 'setting');
  drupal_add_js(drupal_get_path('module', 'uc_order') . '/uc_order.js');

  $output = '';
  if ($search === FALSE) {
//    $output = '<div class="order-overview-form">' . drupal_get_form('uc_order_select_form') . '</div>'
//             . '<div class="order-overview-form">' . drupal_get_form('uc_order_admin_sort_form') . '</div>';
    $output = '';
  }

  $output .= theme('table', $header, $rows, array('class' => 'uc-orders-table'));
  $output .= theme('pager', NULL, variable_get('uc_order_number_displayed', 30), 0);
  return $output;
}