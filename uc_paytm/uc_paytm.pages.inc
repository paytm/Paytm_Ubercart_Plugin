<?php
// $Id: uc_paytm.pages.inc,v 1.1.2.5 2010/08/11 17:52:20 islandusurper Exp $

/**
 * @file
 * paytm menu items.
 *
 */
require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'uc_paytm') . "/Rc43.inc";
require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'uc_paytm') . "/uc_encdec_paytm.php";


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */



// if ($bool == "TRUE") {		

function uc_paytm_complete() {

 /* if (isset($_GET['q'])) {

    $drvalue = explode('DR=',$_GET['q']);
    $DR = preg_replace("/\s/", "+", $drvalue[1]);
    $secret_key = variable_get('uc_paytm_secret_key', ''); // Your Secret Key

    $rc4 = new Crypt_RC4($secret_key);
    $QueryString = base64_decode($DR);
    $rc4->decrypt($QueryString);
    $QueryString = split('&', $QueryString);

    $response = array();
    foreach ($QueryString as $param) {
    $param = split('=', $param);
    $response[$param[0]] = urldecode($param[1]);
    }

  }*/
 
  $orderid_received	= $_POST['ORDERID'];
  $payment_status	= $_POST['RESPCODE'];
  $payment_reason	= $_POST['RESPMSG'];
  $checksum_recv	= $_POST['CHECKSUMHASH'];
  $paramList		= $_POST;
  $bool = "FALSE";
  
  $bool = verifychecksum_e($paramList, variable_get('uc_paytm_secret_key', ''), $checksum_recv);
  
  $order = uc_order_load($orderid_received);


  if ($order === FALSE ) {
    return t('An error has occurred. Please contact us to ensure your order has submitted.');
  }

  
  $payment_amount = $_POST['TXNAMOUNT'];
  $payment_id	  = "";
  if(isset($_POST['TXNID'])){
  $payment_id	  = $_POST['TXNID'];
  }
  $payment_currency = 'INR';

  $context = array(
    'revision' => 'formatted-original',
    'type' => 'amount',
  );
  $options = array(
    'sign' => FALSE,
  );


  if ($payment_status == "01" && $bool == "TRUE" ) {

    $comment = t('Secure paytm Payment ID: @payment_id', array('@payment_id' => $payment_id));
    uc_payment_enter($order->order_id, 'paytm', $payment_amount, $order->uid, NULL, $comment);
    uc_cart_complete_sale($order);
    uc_order_comment_save($order->order_id, 0, t('Payment of @amount @currency submitted through paytm.', array('@amount' => uc_currency_format($payment_amount, FALSE), '@currency' => $payment_currency)), 'order', $payment_reason);

    $order->order_status = 'completed';
    uc_order_save($order);
    drupal_set_message(t('Thank you for your order! Your order has been received and will be processed shortly.'));
    drupal_goto('cart');

  }
  else {
   
    $order->order_status = 'canceled';
    uc_order_save($order);
    drupal_set_message(t('Your paytm payment was cancelled. Please feel free to continue shopping or contact us for assistance.'));
    drupal_goto('cart');

  }

  // Save changes to order
 

  // Add a comment to let sales team know this came in through the site.
 


  if ($order->order_status == 'canceled') {
    drupal_goto('cart');
  }


}
